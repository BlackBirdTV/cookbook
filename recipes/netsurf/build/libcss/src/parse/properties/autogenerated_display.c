/*
 * This file was generated by LibCSS gen_parser 
 * 
 * Generated from:
 *
 * display:CSS_PROP_DISPLAY IDENT:( INHERIT: INLINE:0,DISPLAY_INLINE BLOCK:0,DISPLAY_BLOCK LIST_ITEM:0,DISPLAY_LIST_ITEM RUN_IN:0,DISPLAY_RUN_IN INLINE_BLOCK:0,DISPLAY_INLINE_BLOCK TABLE:0,DISPLAY_TABLE INLINE_TABLE:0,DISPLAY_INLINE_TABLE TABLE_ROW_GROUP:0,DISPLAY_TABLE_ROW_GROUP TABLE_HEADER_GROUP:0,DISPLAY_TABLE_HEADER_GROUP TABLE_FOOTER_GROUP:0,DISPLAY_TABLE_FOOTER_GROUP TABLE_ROW:0,DISPLAY_TABLE_ROW TABLE_COLUMN_GROUP:0,DISPLAY_TABLE_COLUMN_GROUP TABLE_COLUMN:0,DISPLAY_TABLE_COLUMN TABLE_CELL:0,DISPLAY_TABLE_CELL TABLE_CAPTION:0,DISPLAY_TABLE_CAPTION NONE:0,DISPLAY_NONE IDENT:)
 * 
 * Licensed under the MIT License,
 *		  http://www.opensource.org/licenses/mit-license.php
 * Copyright 2010 The NetSurf Browser Project.
 */

#include <assert.h>
#include <string.h>

#include "bytecode/bytecode.h"
#include "bytecode/opcodes.h"
#include "parse/properties/properties.h"
#include "parse/properties/utils.h"

/**
 * Parse display
 *
 * \param c	  Parsing context
 * \param vector  Vector of tokens to process
 * \param ctx	  Pointer to vector iteration context
 * \param result  resulting style
 * \return CSS_OK on success,
 *	   CSS_NOMEM on memory exhaustion,
 *	   CSS_INVALID if the input is not valid
 *
 * Post condition: \a *ctx is updated with the next token to process
 *		   If the input is invalid, then \a *ctx remains unchanged.
 */
css_error css__parse_display(css_language *c,
		const parserutils_vector *vector, int *ctx,
		css_style *result)
{
	int orig_ctx = *ctx;
	css_error error;
	const css_token *token;
	bool match;

	token = parserutils_vector_iterate(vector, ctx);
	if ((token == NULL) || ((token->type != CSS_TOKEN_IDENT))) {
		*ctx = orig_ctx;
		return CSS_INVALID;
	}

	if ((lwc_string_caseless_isequal(token->idata, c->strings[INHERIT], &match) == lwc_error_ok && match)) {
			error = css_stylesheet_style_inherit(result, CSS_PROP_DISPLAY);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[INLINE], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_INLINE);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[BLOCK], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_BLOCK);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[LIST_ITEM], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_LIST_ITEM);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[RUN_IN], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_RUN_IN);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[INLINE_BLOCK], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_INLINE_BLOCK);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[INLINE_TABLE], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_INLINE_TABLE);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_ROW_GROUP], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_ROW_GROUP);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_HEADER_GROUP], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_HEADER_GROUP);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_FOOTER_GROUP], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_FOOTER_GROUP);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_ROW], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_ROW);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_COLUMN_GROUP], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_COLUMN_GROUP);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_COLUMN], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_COLUMN);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_CELL], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_CELL);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[TABLE_CAPTION], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_TABLE_CAPTION);
	} else if ((lwc_string_caseless_isequal(token->idata, c->strings[NONE], &match) == lwc_error_ok && match)) {
			error = css__stylesheet_style_appendOPV(result, CSS_PROP_DISPLAY, 0,DISPLAY_NONE);
	} else {
		error = CSS_INVALID;
	}

	if (error != CSS_OK)
		*ctx = orig_ctx;
	
	return error;
}

