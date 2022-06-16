/* duktape binding generation implementation
 *
 * This file is part of nsgenbind.
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2015 Vincent Sanders <vince@netsurf-browser.org>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>

#include "options.h"
#include "utils.h"
#include "nsgenbind-ast.h"
#include "webidl-ast.h"
#include "ir.h"
#include "duk-libdom.h"

#define NSGENBIND_PREFACE                                               \
    "/* Generated by nsgenbind\n"                                       \
    " *\n"                                                              \
    " * nsgenbind is published under the MIT Licence.\n"                \
    " * nsgenbind is similar to a compiler is a purely transformative tool which\n" \
    " * explicitly makes no copyright claim on this generated output\n" \
    " */"

/* exported interface documented in duk-libdom.h */
int output_tool_preface(FILE* outf)
{
        fprintf(outf, "%s\n", NSGENBIND_PREFACE);

        return 0;
}

/* exported interface documented in duk-libdom.h */
int output_cdata(FILE* outf,
                 struct genbind_node *node,
                 enum genbind_node_type nodetype)
{
        char *cdata;
        int res = 0;

        cdata = genbind_node_gettext(
                genbind_node_find_type(
                        genbind_node_getnode(node),
                        NULL, nodetype));
        if (cdata != NULL) {
                fprintf(outf, "%s", cdata);
                res = 1;
        }
        return res;
}

/* exported interface documented in duk-libdom.h */
int output_ccode(FILE* outf, struct genbind_node *node)
{
        int *line;
        char *filename;

        line = genbind_node_getint(
                genbind_node_find_type(
                        genbind_node_getnode(node),
                        NULL, GENBIND_NODE_TYPE_LINE));

        filename = genbind_node_gettext(
                genbind_node_find_type(
                        genbind_node_getnode(node),
                        NULL, GENBIND_NODE_TYPE_FILE));

        if ((line != NULL) && (filename != NULL)) {
                fprintf(outf, "/* #line %d \"%s\" */\n", *line, filename);
        }

        return output_cdata(outf, node, GENBIND_NODE_TYPE_CDATA);
}

/* exported interface documented in duk-libdom.h */
int output_tool_prologue(FILE* outf)
{
        char *fpath;

        fpath = genb_fpath("binding.h");
        fprintf(outf, "\n#include \"%s\"\n", fpath);
        free(fpath);

        fpath = genb_fpath("private.h");
        fprintf(outf, "#include \"%s\"\n", fpath);
        free(fpath);

        fpath = genb_fpath("prototype.h");
        fprintf(outf, "#include \"%s\"\n", fpath);
        free(fpath);

        return 0;
}


/* exported interface documented in duk-libdom.h */
int output_ctype(FILE *outf, struct genbind_node *node, bool identifier)
{
        const char *type_cdata = NULL;
        struct genbind_node *typename_node;

        typename_node = genbind_node_find_type(genbind_node_getnode(node),
                                               NULL,
                                               GENBIND_NODE_TYPE_NAME);
        while (typename_node != NULL) {
                type_cdata = genbind_node_gettext(typename_node);

                fprintf(outf, "%s", type_cdata);

                typename_node = genbind_node_find_type(
                        genbind_node_getnode(node),
                        typename_node,
                        GENBIND_NODE_TYPE_NAME);

                /* separate all but the last entry with spaces */
                if (typename_node != NULL) {
                        fputc(' ', outf);
                }
        }

        if (identifier) {
            if ((type_cdata != NULL) &&
                (type_cdata[0] != '*') &&
                (type_cdata[0] != ' ')) {
                fputc(' ', outf);
            }

            output_cdata(outf, node, GENBIND_NODE_TYPE_IDENT);
        }

        return 0;
}

/* exported interface documented in duk-libdom.h */
int output_method_cdata(FILE* outf,
                        struct genbind_node *node,
                        enum genbind_method_type sel_method_type)
{
        struct genbind_node *method;

        method = genbind_node_find_type(genbind_node_getnode(node),
                                        NULL,
                                        GENBIND_NODE_TYPE_METHOD);

        while (method != NULL) {
                enum genbind_method_type *method_type;

                method_type = (enum genbind_method_type *)genbind_node_getint(
                        genbind_node_find_type(
                                genbind_node_getnode(method),
                                NULL,
                                GENBIND_NODE_TYPE_METHOD_TYPE));
                if ((method_type != NULL) &&
                    (*method_type == sel_method_type)) {
                        output_cdata(outf, method, GENBIND_NODE_TYPE_CDATA);
                }

                method = genbind_node_find_type(genbind_node_getnode(node),
                                                method,
                                                GENBIND_NODE_TYPE_METHOD);
        }

        return 0;
}

/* exported interface documented in duk-libdom.h */
char *gen_idl2c_name(const char *idlname)
{
        const char *inc;
        char *outc;
        char *name;
        int waslower;

        /* enpty strings are a bad idea */
        if ((idlname == NULL) || (idlname[0] == 0)) {
                return NULL;
        }

        /* allocate result buffer as twice the input length as thats the
         * absolute worst case.
         */
        name = calloc(2, strlen(idlname));

        outc = name;
        inc = idlname;
        waslower = 1;

        /* first character handled separately as inserting a leading underscore
         * is undesirable
         */
        *outc++ = tolower(*inc++);

        /* copy input to output */
        while (*inc != 0) {
                /* ugly hack as html IDL is always prefixed uppercase and needs
                 * an underscore there
                 */
                if ((inc == (idlname + 4)) &&
                    (idlname[0] == 'H') &&
                    (idlname[1] == 'T') &&
                    (idlname[2] == 'M') &&
                    (idlname[3] == 'L') &&
                    (islower(inc[1]) == 0)) {
                        *outc++ = '_';
                }
                if (islower(*inc) != 0) {
                        if (waslower == 0) {
                                /* high to lower case transition */
                                if (((outc - name) <= 3) ||
                                    (*(outc - 3) != '_')) {
                                        *outc = *(outc - 1);
                                        *(outc - 1) = '_';
                                        outc++;
                                }
                        }
                        waslower = 1;
                } else {
                        waslower = 0;
                }
                *outc++ = tolower(*inc++);
        }
        return name;
}
