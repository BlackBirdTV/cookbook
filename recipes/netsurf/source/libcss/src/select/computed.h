/*
 * This file is part of LibCSS
 * Licensed under the MIT License,
 *                http://www.opensource.org/licenses/mit-license.php
 * Copyright 2009 John-Mark Bell <jmb@netsurf-browser.org>
 */

#ifndef css_select_computed_h_
#define css_select_computed_h_

#include <libcss/computed.h>
#include <libcss/hint.h>



struct css_computed_uncommon_i {
/*
 * border_spacing		  1 + 2(4)	  2(4)
 * break_before			  4		  0
 * break_after			  4		  0
 * break_inside			  4		  0
 * clip				  2 + 4(4) + 4	  4(4)
 * column_count			  2		  4
 * column_fill			  2		  0
 * column_gap			  2 + 4		  4
 * column_rule_color		  2		  4
 * column_rule_style		  4		  0
 * column_rule_width		  3 + 4		  4
 * column_span			  2		  0
 * column_width			  2 + 4		  4
 * letter_spacing		  2 + 4		  4
 * outline_color		  2		  4
 * outline_width		  3 + 4		  4
 * word_spacing			  2 + 4		  4
 * 				---		---
 * 				 95 bits	 60 bytes
 *
 * Encode counter_increment and _reset as an array of name, value pairs,
 * terminated with a blank entry.
 *
 * counter_increment		  1		  sizeof(ptr)
 * counter_reset		  1		  sizeof(ptr)
 * 				---		---
 * 				  2 bits	  2sizeof(ptr) bytes
 *
 * Encode cursor uri(s) as an array of string objects, terminated with a
 * blank entry.
 *
 * cursor			  5		  sizeof(ptr)
 * 				---		---
 * 				  5 bits	  sizeof(ptr) bytes
 *
 * Encode content as an array of content items, terminated with a blank entry.
 *
 * content			  2		  sizeof(ptr)
 * 				---		---
 * 				  2 bits	  sizeof(ptr)
 *
 * 				___		___
 * 				108 bits	 62 + 4sizeof(ptr) bytes
 *
 * 				 14 bytes	 62 + 4sizeof(ptr) bytes
 * 				===================
 * 				 74 + 4sizeof(ptr) bytes
 *
 * Bit allocations:
 *
 *    76543210
 *  1 llllllcc	letter-spacing    | outline-color
 *  2 ooooooob	outline-width     | border-spacing
 *  3 bbbbbbbb	border-spacing
 *  4 wwwwwwir	word-spacing      | counter-increment | counter-reset
 *  5 uuuuumm.	cursor            | writing-mode      | <unused>
 *  6 cccccccc	clip
 *  7 cccccccc	clip
 *  8 ccccccoo	clip              | content
 *  9 ccffssss  column_count      | column-fill       | column-rule-style
 * 10 ggggggcc  column-gap        | column-rule-color
 * 11 wwwwwww.  column-rule-width | <unused>
 * 12 sswwwwww  column-span       | column_width
 * 13 bbbbaaaa  break-before      | break-after
 * 14 iiii....  break-inside      | <unused>
 */
	uint8_t bits[14];

	css_fixed border_spacing[2];

	css_fixed clip[4];

	css_fixed letter_spacing;

	css_color outline_color;
	css_fixed outline_width;

	css_fixed word_spacing;

	int32_t column_count;
	css_fixed column_gap;
	css_color column_rule_color;
	css_fixed column_rule_width;
	css_fixed column_width;
};

typedef struct css_computed_uncommon {
	struct css_computed_uncommon_i i;

	css_computed_counter *counter_increment;
	css_computed_counter *counter_reset;

	css_computed_content_item *content;

	lwc_string **cursor;
	struct css_computed_uncommon *next;
	uint32_t count;
	uint32_t bin;
} css_computed_uncommon;

typedef struct css_computed_page {
/*
 * Bit allocations:
 *
 *    76543210
 *  1 aaabbbii	page_break_after | page_break_before | page_break_inside
 *  2 ......wo	widows  | orphans
 */
	uint8_t bits[2];

	int32_t widows;
	int32_t orphans;
} css_computed_page;

struct css_computed_style_i {
/*
 * background_attachment	  2
 * background_repeat		  3
 * border_collapse		  2
 * border_top_style		  4
 * border_right_style		  4
 * border_bottom_style		  4
 * border_left_style		  4
 * caption_side			  2
 * clear			  3
 * direction			  2
 * display			  5
 * empty_cells			  2
 * float			  2
 * font_style			  2
 * font_variant			  2
 * font_weight			  4
 * list_style_position		  2
 * list_style_type		  4
 * overflow			  3
 * outline_style		  4
 * position			  3
 * table_layout			  2
 * text_align			  4
 * text_decoration		  5
 * text_transform		  3
 * unicode_bidi			  2
 * visibility			  2
 * white_space			  3
 * box_sizing			  2
 *				---
 *				 86 bits
 *
 * Colours are 32bits of AARRGGBB
 * Dimensions are encoded as a fixed point value + 4 bits of unit data
 *
 * background_color		  2		  4
 * background_image		  1		  sizeof(ptr)
 * background_position		  1 + 2(4)	  2(4)
 * border_top_color		  2		  4
 * border_right_color		  2		  4
 * border_bottom_color		  2		  4
 * border_left_color		  2		  4
 * border_top_width		  3 + 4		  4
 * border_right_width		  3 + 4		  4
 * border_bottom_width		  3 + 4		  4
 * border_left_width		  3 + 4		  4
 * top				  2 + 4		  4
 * right			  2 + 4		  4
 * bottom			  2 + 4		  4
 * left				  2 + 4		  4
 * color			  1		  4
 * font_size			  4 + 4		  4
 * height			  2 + 4		  4
 * line_height			  2 + 4		  4
 * list_style_image		  1		  sizeof(ptr)
 * margin_top			  2 + 4		  4
 * margin_right			  2 + 4		  4
 * margin_bottom		  2 + 4		  4
 * margin_left			  2 + 4		  4
 * max_height			  2 + 4		  4
 * max_width			  2 + 4		  4
 * min_height			  1 + 4		  4
 * min_width			  1 + 4		  4
 * padding_top			  1 + 4		  4
 * padding_right		  1 + 4		  4
 * padding_bottom		  1 + 4		  4
 * padding_left			  1 + 4		  4
 * text_indent			  1 + 4		  4
 * vertical_align		  4 + 4		  4
 * width			  2 + 4		  4
 * z_index			  2		  4
 * 				---		---
 *				181 bits	140 + 2sizeof(ptr) bytes
 *
 * Encode font family as an array of string objects, terminated with a
 * blank entry.
 *
 * font_family			  3		  sizeof(ptr)
 * 				---		---
 * 				  3 bits	  sizeof(ptr)
 *
 * Encode quotes as an array of string objects, terminated with a blank entry.
 *
 * quotes			  1		  sizeof(ptr)
 * 				---		---
 * 				  1 bit		  sizeof(ptr) bytes
 *
 * 				___		___
 *				269 bits	140 + 4sizeof(ptr) bytes
 *
 *				 34 bytes	140 + 4sizeof(ptr) bytes
 *				===================
 *				174 + 4sizeof(ptr) bytes
 *
 * Bit allocations:
 *
 *    76543210
 *  1 vvvvvvvv	vertical-align
 *  2 ffffffff	font-size
 *  3 ttttttti	border-top-width    | background-image
 *  4 rrrrrrrc	border-right-width  | color
 *  5 bbbbbbbl	border-bottom-width | list-style-image
 *  6 lllllllq	border-left-width   | quotes
 *  7 ttttttcc	top                 | border-top-color
 *  8 rrrrrrcc	right               | border-right-color
 *  9 bbbbbbcc	bottom              | border-bottom-color
 * 10 llllllcc	left                | border-left-color
 * 11 hhhhhhbb	height              | background-color
 * 12 llllllzz	line-height         | z-index
 * 13 ttttttbb	margin-top          | background-attachment
 * 14 rrrrrrbb	margin-right        | border-collapse
 * 15 bbbbbbcc	margin-bottom       | caption-side
 * 16 lllllldd	margin-left         | direction
 * 17 mmmmmmee	max-height          | empty-cells
 * 18 mmmmmmff	max-width           | float
 * 19 wwwwwwff	width               | font-style
 * 20 mmmmmbbb	min-height          | background-repeat
 * 21 mmmmmccc	min-width           | clear
 * 22 tttttxxx	padding-top         | overflow-x
 * 23 rrrrrppp	padding-right       | position
 * 24 bbbbboss	padding-bottom      | opacity               | box-sizing
 * 25 lllllttt	padding-left        | text-transform
 * 26 tttttwww	text-indent         | white-space
 * 27 bbbbbbbb	background-position
 * 28 bdddddff	background-position | display               | font-variant
 * 29 tttttfff	text-decoration     | font-family
 * 30 ttttrrrr	border-top-style    | border-right-style
 * 31 bbbbllll	border-bottom-style | border-left-style
 * 32 ffffllll	font-weight         | list-style-type
 * 33 oooottuu	outline-style       | table-layout          | unicode-bidi
 * 34 vvlltttt	visibility          | list-style-position   | text-align
 * 35 yyy.....	overflow-y          | <unused>
 */
	uint8_t bits[35];

	uint8_t unused[1];

	css_color background_color;
	lwc_string *background_image;
	css_fixed background_position[2];

	css_color border_color[4];
	css_fixed border_width[4];

	css_fixed top;
	css_fixed right;
	css_fixed bottom;
	css_fixed left;

	css_color color;

	css_fixed font_size;

	css_fixed height;

	css_fixed line_height;

	lwc_string *list_style_image;

	css_fixed margin[4];

	css_fixed max_height;
	css_fixed max_width;

	css_fixed min_height;
	css_fixed min_width;

	css_fixed opacity;

	css_fixed padding[4];

	css_fixed text_indent;

	css_fixed vertical_align;

	css_fixed width;

	int32_t z_index;

	css_computed_uncommon *uncommon;/**< Uncommon properties */
	void *aural;			/**< Aural properties */
};

struct css_computed_style {
	struct css_computed_style_i i;

	lwc_string **font_family;

	lwc_string **quotes;
	css_computed_page *page;	/**< Page properties */
	struct css_computed_style *next;
	uint32_t count;
	uint32_t bin;
};


/**
 * Take a new reference to a computed style
 *
 * \param style  The style to take a new reference to.
 * \return The new computed style reference
 */
static inline css_computed_style * css__computed_style_ref(
		css_computed_style *style)
{
	if (style == NULL)
		return NULL;

	if (style->i.uncommon != NULL) {
		style->i.uncommon->count++;
	}

	style->count++;
	return style;
}

css_error css__computed_style_create(css_computed_style **result);

css_error css__computed_style_initialise(css_computed_style *style,
		struct css_select_handler *handler, void *pw);


css_error css__computed_uncommon_destroy(css_computed_uncommon *uncommon);

css_error css__compute_absolute_values(const css_computed_style *parent,
		css_computed_style *style,
		css_error (*compute_font_size)(void *pw,
			const css_hint *parent, css_hint *size),
		void *pw);

#endif
