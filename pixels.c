/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Authors: Santiago Lizardo <santiagolizardo@php.net>                  |
  |          Remi Collet <remi@php.net>                                  |
  +----------------------------------------------------------------------+
*/


#include "pixels.h"
#include "zend_interfaces.h"

static zend_class_entry *php_sdl_color_ce;
static zend_object_handlers php_sdl_color_handlers;
struct php_sdl_color {
	zend_object   zo;
};

static zend_class_entry *php_sdl_palette_ce;
static zend_object_handlers php_sdl_palette_handlers;
struct php_sdl_palette {
	zend_object   zo;
	SDL_Palette   *palette;
	Uint32        flags;
};

static zend_class_entry *php_sdl_pixelformat_ce;
static zend_object_handlers php_sdl_pixelformat_handlers;
struct php_sdl_pixelformat {
	zend_object      zo;
	SDL_PixelFormat *format;
	Uint32           flags;
};

static zend_class_entry *php_sdl_pixels_ce;
static zend_object_handlers php_sdl_pixels_handlers;
struct php_sdl_pixels {
	zend_object      zo;
	SDL_Pixels       pixels;
	Uint32           flags;
};

zend_class_entry *get_php_sdl_color_ce(void)
{
	return php_sdl_color_ce;
}

zend_class_entry *get_php_sdl_pixelformat_ce(void)
{
	return php_sdl_pixelformat_ce;
}

zend_class_entry *get_php_sdl_palette_ce(void)
{
	return php_sdl_palette_ce;
}

zend_class_entry *get_php_sdl_pixels_ce(void)
{
	return php_sdl_pixels_ce;
}

#define FETCH_PALETTE(__ptr, __id, __check) \
{ \
        intern = (struct php_sdl_palette *)Z_OBJ_P(__id TSRMLS_CC);\
        __ptr = intern->palette; \
        if (__check && !__ptr) {\
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid %s object", intern->zo.ce->name);\
                RETURN_FALSE;\
        }\
}

#define FETCH_PIXELFORMAT(__ptr, __id, __check) \
{ \
        intern = (struct php_sdl_pixelformat *)Z_OBJ_P(__id TSRMLS_CC);\
        __ptr = intern->format; \
        if (__check && !__ptr) {\
                php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid %s object", intern->zo.ce->name);\
                RETURN_FALSE;\
        }\
}

zend_bool sdl_color_to_zval(SDL_Color *color, zval *value TSRMLS_DC)
{
	if (color) {
		object_init_ex(value, php_sdl_color_ce);
		zend_update_property_long(php_sdl_color_ce, value, "r", 1, color->r TSRMLS_CC);
		zend_update_property_long(php_sdl_color_ce, value, "g", 1, color->g TSRMLS_CC);
		zend_update_property_long(php_sdl_color_ce, value, "b", 1, color->b TSRMLS_CC);
		zend_update_property_long(php_sdl_color_ce, value, "a", 1, color->a TSRMLS_CC);

		return 1;
	}
	ZVAL_NULL(value);
	return 0;
}

zend_bool zval_to_sdl_color(zval *value, SDL_Color *color TSRMLS_DC)
{
	if (Z_TYPE_P(value) == IS_OBJECT && Z_OBJCE_P(value) == php_sdl_color_ce) {
		zval *val, rv;

		val = zend_read_property(php_sdl_color_ce, value, "r", 1, 0, &rv TSRMLS_CC);
		convert_to_long(val);
		Z_LVAL_P(val) = color->r = (Uint8)Z_LVAL_P(val);

		val = zend_read_property(php_sdl_color_ce, value, "g", 1, 0, &rv TSRMLS_CC);
		convert_to_long(val);
		Z_LVAL_P(val) = color->g = (Uint8)Z_LVAL_P(val);

		val = zend_read_property(php_sdl_color_ce, value, "b", 1, 0, &rv TSRMLS_CC);
		convert_to_long(val);
		Z_LVAL_P(val) = color->b = (Uint8)Z_LVAL_P(val);

		val = zend_read_property(php_sdl_color_ce, value, "a", 1, 0, &rv TSRMLS_CC);
		convert_to_long(val);
		Z_LVAL_P(val) = color->a = (Uint8)Z_LVAL_P(val);

		return 1;
	}
	/* create an empty color */
	memset(color, 0, sizeof(SDL_Color));
	return 0;
}


/* {{{ sdl_palette_to_zval */
zend_bool sdl_palette_to_zval(SDL_Palette *palette, zval *z_val, Uint32 flags TSRMLS_DC)
{
	if (palette) {
		struct php_sdl_palette *intern;

		object_init_ex(z_val, php_sdl_palette_ce);
		intern = (struct php_sdl_palette *)Z_OBJ_P(z_val TSRMLS_CC);
		intern->palette = palette;
		intern->flags = flags;

		return 1;
	}
	ZVAL_NULL(z_val);
	return 0;
}
/* }}} */


/* {{{ sdl_pixelformat_to_zval */
zend_bool sdl_pixelformat_to_zval(SDL_PixelFormat *format, zval *z_val, Uint32 flags TSRMLS_DC)
{
	if (format) {
		struct php_sdl_pixelformat *intern;

		object_init_ex(z_val, php_sdl_pixelformat_ce);
		intern = (struct php_sdl_pixelformat *)Z_OBJ_P(z_val TSRMLS_CC);
		intern->format = format;
		intern->flags = flags;

		return 1;
	}

	ZVAL_NULL(z_val);
	return 0;
}
/* }}} */

/* {{{ sdl_pixels_to_zval */
zend_bool sdl_pixels_to_zval(SDL_Pixels *pixels, zval *z_val, Uint32 flags TSRMLS_DC)
{
	if (pixels) {
		struct php_sdl_pixels *intern;

		object_init_ex(z_val, php_sdl_pixels_ce);
		intern = (struct php_sdl_pixels *)Z_OBJ_P(z_val TSRMLS_CC);
		intern->pixels = *pixels;
		intern->flags  = flags;

		return 1;
	}
	ZVAL_NULL(z_val);
	return 0;
}
/* }}} */

/* {{{ zval_to_sdl_pixelformat */
SDL_PixelFormat *zval_to_sdl_pixelformat(zval *z_val TSRMLS_DC)
{
	if (Z_TYPE_P(z_val) == IS_OBJECT && Z_OBJCE_P(z_val) == php_sdl_pixelformat_ce) {
		struct php_sdl_pixelformat *intern;

		intern = (struct php_sdl_pixelformat *)Z_OBJ_P(z_val TSRMLS_CC);
		return intern->format;
		}
	return NULL;
}
/* }}} */

/* {{{ zval_to_sdl_pixels */
SDL_Pixels *zval_to_sdl_pixels(zval *z_val TSRMLS_DC)
{
	if (Z_TYPE_P(z_val) == IS_OBJECT && Z_OBJCE_P(z_val) == php_sdl_pixels_ce) {
		struct php_sdl_pixels *intern;

		intern = (struct php_sdl_pixels *)Z_OBJ_P(z_val TSRMLS_CC);
		return &intern->pixels;
		}
	return NULL;
}
/* }}} */

/* {{{ zval_to_sdl_palette */
SDL_Palette *zval_to_sdl_palette(zval *z_val TSRMLS_DC)
{
	if (Z_TYPE_P(z_val) == IS_OBJECT && Z_OBJCE_P(z_val) == php_sdl_palette_ce) {
		struct php_sdl_palette *intern;

		intern = (struct php_sdl_palette *)Z_OBJ_P(z_val TSRMLS_CC);
		return intern->palette;
		}
	return NULL;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Color__construct, 0, 0, 4)
       ZEND_ARG_INFO(0, r)
       ZEND_ARG_INFO(0, g)
       ZEND_ARG_INFO(0, b)
       ZEND_ARG_INFO(0, a)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_Color::__construct(int r, int g, int b, int a) */
static PHP_METHOD(SDL_Color, __construct)
{
	long r, g, b, a;
	zend_error_handling error_handling;

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "llll", &r, &g, &b, &a)) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	zend_update_property_long(php_sdl_color_ce, getThis(), "r", 1, r&255 TSRMLS_CC);
	zend_update_property_long(php_sdl_color_ce, getThis(), "g", 1, g&255 TSRMLS_CC);
	zend_update_property_long(php_sdl_color_ce, getThis(), "b", 1, b&255 TSRMLS_CC);
	zend_update_property_long(php_sdl_color_ce, getThis(), "a", 1, a&255 TSRMLS_CC);
}
/* }}} */


/* {{{ proto SDL_Color::__toString() */
static PHP_METHOD(SDL_Color, __toString)
{
	char *buf;
	SDL_Color color;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	zval_to_sdl_color(getThis(), &color TSRMLS_CC);
	spprintf(&buf, 100, "SDL_Color(%u,%u,%u,%u)", color.r, color.g, color.b, color.a);
	RETVAL_STRING(buf);
}
/* }}} */

/* {{{ proto string SDL_GetPixelFormatName(int format)

 * \brief Get the human readable name of a pixel format
 extern DECLSPEC const char* SDLCALL SDL_GetPixelFormatName(Uint32 format);
 */
PHP_FUNCTION(SDL_GetPixelFormatName)
{
	long format;
	const char *name;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &format) == FAILURE) {
		RETURN_FALSE;
	}
	name = SDL_GetPixelFormatName((Uint32)format);
	if (!name) {
		RETURN_FALSE;
	}
	RETURN_STRING(name);
}
/* }}} */

/* {{{ proto bool SDL_PixelFormatEnumToMasks(int format, int &bpp, int &Rmask, int &Gmask, int &Bmask, int &Amask)

 *  \brief Convert one of the enumerated pixel formats to a bpp and RGBA masks.
 *
 *  \return SDL_TRUE, or SDL_FALSE if the conversion wasn't possible.
 *
 *  \sa SDL_MasksToPixelFormatEnum()
 extern DECLSPEC SDL_bool SDLCALL SDL_PixelFormatEnumToMasks(Uint32 format,
                                                             int *bpp,
                                                             Uint32 * Rmask,
                                                             Uint32 * Gmask,
                                                             Uint32 * Bmask,
                                                             Uint32 * Amask);
 */
PHP_FUNCTION(SDL_PixelFormatEnumToMasks)
{
	zval *z_bpp, *z_rmask, *z_gmask, *z_bmask, *z_amask;
	long format;
	int bpp;
	Uint32 rmask, gmask, bmask, amask;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lzzzzz", &format, &z_bpp, &z_rmask, &z_gmask, &z_bmask, &z_amask) == FAILURE) {
		RETURN_FALSE;
	}
	if (SDL_PixelFormatEnumToMasks((Uint32)format, &bpp, &rmask, &gmask, &bmask, &amask)) {
		zval_dtor(z_bpp);
		ZVAL_LONG(z_bpp, bpp);
		zval_dtor(z_rmask);
		ZVAL_LONG(z_rmask, rmask);
		zval_dtor(z_gmask);
		ZVAL_LONG(z_gmask, gmask);
		zval_dtor(z_bmask);
		ZVAL_LONG(z_bmask, bmask);
		zval_dtor(z_amask);
		ZVAL_LONG(z_amask, amask);

		RETURN_TRUE;
	}
	RETURN_FALSE;;
}
/* }}} */


/* {{{ proto bool SDL_MasksToPixelFormatEnum(int format, int bpp, int Rmask, int Gmask, int Bmask, int Amask)
 *  \brief Convert a bpp and RGBA masks to an enumerated pixel format.
 *
 *  \return The pixel format, or ::SDL_PIXELFORMAT_UNKNOWN if the conversion
 *          wasn't possible.
 *
 *  \sa SDL_PixelFormatEnumToMasks()
 extern DECLSPEC Uint32 SDLCALL SDL_MasksToPixelFormatEnum(int bpp,
                                                           Uint32 Rmask,
                                                           Uint32 Gmask,
                                                           Uint32 Bmask,
                                                           Uint32 Amask);
 */
PHP_FUNCTION(SDL_MasksToPixelFormatEnum)
{
	long bpp, rmask, gmask, bmask, amask;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lllll", &bpp, &rmask, &gmask, &bmask, &amask) == FAILURE) {
		RETURN_FALSE;
	}

	RETURN_LONG(SDL_MasksToPixelFormatEnum((int)bpp, (Uint32)rmask, (Uint32)gmask, (Uint32)bmask, (Uint32)amask));
}
/* }}} */

/* {{{ proto SDL_PixelFormat SDL_AllocFormat(int ncolors)

 *  \brief Create an SDL_PixelFormat structure from a pixel format enum.
 extern DECLSPEC SDL_PixelFormat * SDLCALL SDL_AllocFormat(Uint32 pixel_format);
 */
PHP_FUNCTION(SDL_AllocFormat)
{
	long index;
	SDL_PixelFormat *format;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &index)) {
		return;
	}
	format = SDL_AllocFormat((Uint32)index);
	sdl_pixelformat_to_zval(format, return_value, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto SDL_PixelFormat::__construct(format) */
static PHP_METHOD(SDL_PixelFormat, __construct)
{
	struct php_sdl_pixelformat *intern;
	long format;
	zend_error_handling error_handling;

	intern = (struct php_sdl_pixelformat *)Z_OBJ_P(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &format)) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	intern->format = SDL_AllocFormat((Uint32)format);
	if (intern->format) {
		intern->flags = 0;
	} else {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), SDL_GetError(), 0 TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto SDL_PixelFormat::__toString() */
static PHP_METHOD(SDL_PixelFormat, __toString)
{
	struct php_sdl_pixelformat *intern;
	char *buf;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = (struct php_sdl_pixelformat *)Z_OBJ_P(getThis() TSRMLS_CC);
	spprintf(&buf, 100, "SDL_PixelFormat(%s)", SDL_GetPixelFormatName(intern->format->format));
	RETVAL_STRING(buf);
}
/* }}} */


/* {{{ proto void SDL_FreeFormat(SDL_PixelFormat format)

 *  \brief Free an SDL_PixelFormat structure.
 extern DECLSPEC void SDLCALL SDL_FreeFormat(SDL_PixelFormat *format);
 */
PHP_FUNCTION(SDL_FreeFormat)
{
	struct php_sdl_pixelformat *intern;
	zval *object;
	SDL_PixelFormat *format;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, php_sdl_pixelformat_ce) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, object, 1);

	SDL_FreeFormat(intern->format);
	intern->format = NULL;
}
/* }}} */


/* {{{ proto SDL_Palette SDL_AllocPalette(int ncolors)

 *  \brief Create a palette structure with the specified number of color
 *         entries.
 *
 *  \return A new palette, or NULL if there wasn't enough memory.
 *
 *  \note The palette entries are initialized to white.
 *
 *  \sa SDL_FreePalette()
 extern DECLSPEC SDL_Palette *SDLCALL SDL_AllocPalette(int ncolors);
*/
PHP_FUNCTION(SDL_AllocPalette)
{
	long ncolors;
	SDL_Palette *palette;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ncolors)) {
		return;
	}
	palette = SDL_AllocPalette(ncolors);
	sdl_palette_to_zval(palette, return_value, 0 TSRMLS_CC);
}
/* }}} */

/* {{{ proto SDL_Palette::__construct(ncolors) */
static PHP_METHOD(SDL_Palette, __construct)
{
	struct php_sdl_palette *intern;
	long ncolors;
	zend_error_handling error_handling;

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &ncolors)) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	intern->palette = SDL_AllocPalette(ncolors);
	if (intern->palette) {
		intern->flags = 0;
	} else {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), SDL_GetError(), 0 TSRMLS_CC);
	}
}
/* }}} */


/* {{{ proto SDL_Palette::__toString() */
static PHP_METHOD(SDL_Palette, __toString)
{
	struct php_sdl_palette *intern;
	char *buf;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);
	spprintf(&buf, 100, "SDL_Palette(%d)", intern->palette ? intern->palette->ncolors : 0);
	RETVAL_STRING(buf);
}
/* }}} */


/* {{{ proto SDL_Palette, count(void)  */
static PHP_METHOD(SDL_Palette, count)
{
	struct php_sdl_palette *intern;

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->palette ? intern->palette->ncolors : 0);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Palette_offset, 0, 0, 1)
       ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_Palette, offsetExists(int offset) */
PHP_METHOD(SDL_Palette, offsetExists)
{
	struct php_sdl_palette *intern;
	long offset;

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}
	if (!intern->palette || offset < 0 || offset >= (intern->palette->ncolors)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto SDL_Palette, offsetGet(int offset) */
PHP_METHOD(SDL_Palette, offsetGet)
{
	struct php_sdl_palette *intern;
	long offset;

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}
	if (!intern->palette || offset < 0 || offset >= (intern->palette->ncolors)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid offset in SDL_Pixels", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	sdl_color_to_zval(intern->palette->colors+offset, return_value TSRMLS_CC);
}
/* }}} */

/* {{{ proto SDL_Palette, offsetUnset(int offset) */
PHP_METHOD(SDL_Palette, offsetUnset)
{
	struct php_sdl_palette *intern;
	long offset;
	SDL_Color color;

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}
	if (!intern->palette || offset < 0 || offset >= (intern->palette->ncolors)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid offset in SDL_Pixels", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	memset(&color, 0, sizeof(color));
	SDL_SetPaletteColors(intern->palette, &color, (int)offset, 1);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Palette_offsetSet, 0, 0, 2)
       ZEND_ARG_INFO(0, offset)
       ZEND_ARG_INFO(0, color)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_Palette, offsetSet(int offset, int value) */
PHP_METHOD(SDL_Palette, offsetSet)
{
	struct php_sdl_palette *intern;
	long offset;
	zval *z_color;
	SDL_Color color;

	intern = (struct php_sdl_palette *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lO", &offset, &z_color, php_sdl_color_ce) == FAILURE) {
		return;
	}
	if (!intern->palette || offset < 0 || offset >= (intern->palette->ncolors)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid offset in SDL_Pixels", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	zval_to_sdl_color(z_color, &color TSRMLS_CC);
	SDL_SetPaletteColors(intern->palette, &color, (int)offset, 1);
}
/* }}} */

/* {{{ proto int SDL_SetPixelFormatPalette(SDL_PixelFormat format, SDL_Palette palette);

 *  \brief Set the palette for a pixel format structure.
 extern DECLSPEC int SDLCALL SDL_SetPixelFormatPalette(SDL_PixelFormat * format,
                                                       SDL_Palette *palette);
 */
PHP_FUNCTION(SDL_SetPixelFormatPalette)
{
	struct php_sdl_palette *intern;
	zval *z_format, *z_palette;
	SDL_Palette *palette;
	SDL_PixelFormat *format;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OO", &z_format, php_sdl_pixelformat_ce, &z_palette, php_sdl_palette_ce) == FAILURE) {
		return;
	}
	FETCH_PALETTE(palette, z_palette, 1);
	format = zval_to_sdl_pixelformat(z_format TSRMLS_CC);

	RETURN_LONG(SDL_SetPixelFormatPalette(format, palette));
}
/* }}} */


/* {{{ proto int SDL_SetPaletteColors(SDL_Palette palette, array colors, int first, int ncolors)

 *  \brief Set a range of colors in a palette.
 *
 *  \param palette    The palette to modify.
 *  \param colors     An array of colors to copy into the palette.
 *  \param firstcolor The index of the first palette entry to modify.
 *  \param ncolors    The number of entries to modify.
 *
 *  \return 0 on success, or -1 if not all of the colors could be set.
 extern DECLSPEC int SDLCALL SDL_SetPaletteColors(SDL_Palette * palette,
                                                  const SDL_Color * colors,
                                                  int firstcolor, int ncolors);
 */
PHP_FUNCTION(SDL_SetPaletteColors)
{
	struct php_sdl_palette *intern;
	zval *object, *z_colors, *z_color;
	SDL_Palette *palette;
	SDL_Color *colors;
	int i, nb;
	long first=0, ncolors=0, count;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oa|ll", &object, php_sdl_palette_ce, &z_colors, &first, &ncolors) == FAILURE) {
		return;
	}
	FETCH_PALETTE(palette, object, 1);
	count = zend_hash_next_free_element(Z_ARRVAL_P(z_colors));

	if (ncolors <= 0) {
		ncolors = count;
	} else if (ncolors > count) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid color number, %ld will be used instead of %ld", count, ncolors);
		ncolors = count;
	}
	if (first >= intern->palette->ncolors) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid first color index, %ld >= %d", first, intern->palette->ncolors);
		RETURN_LONG(-1);
	}
	if (first+ncolors > intern->palette->ncolors) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Invalid color number, %ld > %d", first+ncolors, intern->palette->ncolors);
		RETURN_LONG(-1);
	}
	colors = emalloc(sizeof(SDL_Color) * ncolors);
	for (nb=i=0 ; i<ncolors ; i++) {
		if (!(z_color = zend_hash_index_find(Z_ARRVAL_P(z_colors), i))) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Unable to find colors[%d]", i);

		} else if (Z_TYPE_P(z_color) != IS_OBJECT || Z_OBJCE_P(z_color) != php_sdl_color_ce) {
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "colors[%d] is not a SDL_Color object", i);
		} else {
			zval_to_sdl_color(z_color, colors+nb TSRMLS_CC);
			nb++;
		}
	}
	if (nb) {
		RETVAL_LONG(SDL_SetPaletteColors(palette, colors, (int)first, nb));
	} else {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "no color in provided array");
		RETVAL_LONG(-1);
	}
	efree(colors);
}
/* }}} */


/* {{{ proto void SDL_FreePalette(SDL_Palette palette)

 *  \brief Free a palette created with SDL_AllocPalette().
 *
 *  \sa SDL_AllocPalette()
 extern DECLSPEC void SDLCALL SDL_FreePalette(SDL_Palette * palette);
 */
PHP_FUNCTION(SDL_FreePalette)
{
	struct php_sdl_palette *intern;
	zval *object;
	SDL_Palette *palette;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "O", &object, php_sdl_palette_ce) == FAILURE) {
		return;
	}
	FETCH_PALETTE(palette, object, 1);

	SDL_FreePalette(intern->palette);
	intern->palette = NULL;
}
/* }}} */



/* {{{ proto int SDL_MapRGB(SDL_PixelFormat format, int r, int g, int b)

 *  \brief Maps an RGB triple to an opaque pixel value for a given pixel format.
 *
 *  \sa SDL_MapRGBA
 extern DECLSPEC Uint32 SDLCALL SDL_MapRGB(const SDL_PixelFormat * format,
                                           Uint8 r, Uint8 g, Uint8 b);
 */
PHP_FUNCTION(SDL_MapRGB)
{
	struct php_sdl_pixelformat *intern;
	zval *z_format;
	SDL_PixelFormat *format;
	long r, g, b;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olll", &z_format, php_sdl_pixelformat_ce, &r, &g, &b) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, z_format, 1);

	RETURN_LONG(SDL_MapRGB(format, (Uint8)r, (Uint8)g, (Uint8)b));
}
/* }}} */



/* {{{ proto int SDL_MapRGBA(SDL_PixelFormat format, int r, int g, int b, int a)

 *  \brief Maps an RGBA quadruple to a pixel value for a given pixel format.
 *
 *  \sa SDL_MapRGB
 extern DECLSPEC Uint32 SDLCALL SDL_MapRGBA(const SDL_PixelFormat * format,
                                            Uint8 r, Uint8 g, Uint8 b,
                                            Uint8 a);
 */
PHP_FUNCTION(SDL_MapRGBA)
{
	struct php_sdl_pixelformat *intern;
	zval *z_format;
	SDL_PixelFormat *format;
	long r, g, b, a;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Ollll", &z_format, php_sdl_pixelformat_ce, &r, &g, &b, &a) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, z_format, 1);

	RETURN_LONG(SDL_MapRGBA(format, (Uint8)r, (Uint8)g, (Uint8)b, (Uint8)a));
}
/* }}} */

/* {{{ proto void SDL_GetRGB(int pixel, SDL_PixelFormat format, int &r, int &g, int &b)

 *  \brief Get the RGB components from a pixel of the specified format.
 *
 *  \sa SDL_GetRGBA
 extern DECLSPEC void SDLCALL SDL_GetRGB(Uint32 pixel,
                                         const SDL_PixelFormat * format,
                                         Uint8 * r, Uint8 * g, Uint8 * b);
 */
PHP_FUNCTION(SDL_GetRGB)
{
	struct php_sdl_pixelformat *intern;
	zval *z_format, *z_r, *z_g, *z_b;
	SDL_PixelFormat *format;
	Uint8 r, g, b;
	long pix;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lOzzz", &pix, &z_format, php_sdl_pixelformat_ce, &z_r, &z_g, &z_b) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, z_format, 1);
	SDL_GetRGB((Uint32)pix, format, &r, &g, &b);
	zval_dtor(z_r);
	ZVAL_LONG(z_r, r);
	zval_dtor(z_g);
	ZVAL_LONG(z_g, g);
	zval_dtor(z_b);
	ZVAL_LONG(z_b, b);
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_PixelFormat_GetRGB, 0, 0, 4)
       ZEND_ARG_INFO(0, pixel)
       ZEND_ARG_INFO(1, r)
       ZEND_ARG_INFO(1, g)
       ZEND_ARG_INFO(1, b)
ZEND_END_ARG_INFO()

/* {{{ proto int SDL_PixelFormat::GetRGB(int pixel, int &r, int &g, int &b)

 Duplicate implementation because for param order
*/
PHP_METHOD(SDL_PixelFormat, GetRGB)
{
	struct php_sdl_pixelformat *intern;
	zval *z_format, *z_r, *z_g, *z_b;
	SDL_PixelFormat *format;
	Uint8 r, g, b;
	long pix;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olzzz", &z_format, php_sdl_pixelformat_ce, &pix, &z_r, &z_g, &z_b) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, z_format, 1);
	SDL_GetRGB((Uint32)pix, format, &r, &g, &b);
	zval_dtor(z_r);
	ZVAL_LONG(z_r, r);
	zval_dtor(z_g);
	ZVAL_LONG(z_g, g);
	zval_dtor(z_b);
	ZVAL_LONG(z_b, b);
}
/* }}} */

/* {{{ proto void SDL_GetRGBA(int pixel, SDL_PixelFormat format, int &r, int &g, int &b, int &a)

 *  \brief Get the RGBA components from a pixel of the specified format.
 *
 *  \sa SDL_GetRGB
 extern DECLSPEC void SDLCALL SDL_GetRGBA(Uint32 pixel,
                                          const SDL_PixelFormat * format,
                                          Uint8 * r, Uint8 * g, Uint8 * b,
                                          Uint8 * a);
 */
PHP_FUNCTION(SDL_GetRGBA)
{
	struct php_sdl_pixelformat *intern;
	zval *z_format, *z_r, *z_g, *z_b, *z_a;
	SDL_PixelFormat *format;
	Uint8 r, g, b, a;
	long pix;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lOzzzz", &pix, &z_format, php_sdl_pixelformat_ce, &z_r, &z_g, &z_b, &z_a) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, z_format, 1);
	SDL_GetRGBA((Uint32)pix, format, &r, &g, &b, &a);
	zval_dtor(z_r);
	ZVAL_LONG(z_r, r);
	zval_dtor(z_g);
	ZVAL_LONG(z_g, g);
	zval_dtor(z_b);
	ZVAL_LONG(z_b, b);
	zval_dtor(z_a);
	ZVAL_LONG(z_a, a);
}
/* }}} */


/* {{{ proto int SDL_PixelFormat::GetRGBA(int pixel, int &r, int &g, int &b, int &a)

 Duplicate implementation because for param order
*/
PHP_METHOD(SDL_PixelFormat, GetRGBA)
{
	struct php_sdl_pixelformat *intern;
	zval *z_format, *z_r, *z_g, *z_b, *z_a;
	SDL_PixelFormat *format;
	Uint8 r, g, b, a;
	long pix;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olzzzz", &z_format, php_sdl_pixelformat_ce, &pix, &z_r, &z_g, &z_b, &z_a) == FAILURE) {
		return;
	}
	FETCH_PIXELFORMAT(format, z_format, 1);
	SDL_GetRGBA((Uint32)pix, format, &r, &g, &b, &a);
	zval_dtor(z_r);
	ZVAL_LONG(z_r, r);
	zval_dtor(z_g);
	ZVAL_LONG(z_g, g);
	zval_dtor(z_b);
	ZVAL_LONG(z_b, b);
	zval_dtor(z_a);
	ZVAL_LONG(z_a, a);
}
/* }}} */


/* {{{ proto void SDL_CalculateGammaRamp(float gamma, array &ramp)

 *  \brief Calculate a 256 entry gamma ramp for a gamma value.
 extern DECLSPEC void SDLCALL SDL_CalculateGammaRamp(float gamma, Uint16 * ramp);
 */
PHP_FUNCTION(SDL_CalculateGammaRamp)
{
	double gamma;
	zval *z_ramp;
	Uint16 ramp[256];
	int i;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "dz", &gamma, &z_ramp) == FAILURE) {
		RETURN_FALSE;
	}
	SDL_CalculateGammaRamp((float)gamma, ramp);
	zval_dtor(z_ramp);
	array_init(z_ramp);
	for (i=0 ; i<256 ; i++) {
		add_next_index_long(z_ramp, ramp[i]);
	}
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Pixels__construct, 0, 0, 2)
       ZEND_ARG_INFO(0, pitch)
       ZEND_ARG_INFO(0, h)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_Pixels::__construct(int pitch, int h) */
static PHP_METHOD(SDL_Pixels, __construct)
{
	struct php_sdl_pixels *intern;
	long pitch, h;
	zend_error_handling error_handling;

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);

	zend_replace_error_handling(EH_THROW, NULL, &error_handling TSRMLS_CC);
	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &pitch, &h)) {
		zend_restore_error_handling(&error_handling TSRMLS_CC);
		return;
	}
	zend_restore_error_handling(&error_handling TSRMLS_CC);

	if (php_sdl_check_overflow(pitch, h, 1)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid size", 0 TSRMLS_CC);
	} else {
		if (pitch & 3) {
			pitch = (pitch + 3) & ~3;
			php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Pitch set to %ld", pitch);
		}
		intern->pixels.pixels = ecalloc(pitch, h);
		intern->pixels.pitch  = pitch;
		intern->pixels.h      = h;
	}
}
/* }}} */


/* {{{ proto SDL_Pixels::__toString() */
static PHP_METHOD(SDL_Pixels, __toString)
{
	struct php_sdl_pixels *intern;
	char *buf;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);
	spprintf(&buf, 100, "SDL_Pixels(%d,%d)", intern->pixels.pitch, intern->pixels.h);
	RETVAL_STRING(buf);
}
/* }}} */


/* {{{ proto SDL_Pixels, count(void) */
static PHP_METHOD(SDL_Pixels, count)
{
	struct php_sdl_pixels *intern;

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	RETURN_LONG(intern->pixels.h * intern->pixels.pitch);
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Pixels_offset, 0, 0, 1)
       ZEND_ARG_INFO(0, offset)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_Pixels, offsetExists(int offset) */
PHP_METHOD(SDL_Pixels, offsetExists)
{
	struct php_sdl_pixels *intern;
	long offset;

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}
	if (offset < 0 || offset >= (intern->pixels.h * intern->pixels.pitch)) {
		RETURN_FALSE;
	}
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto SDL_Pixels, offsetGet(int offset) */
PHP_METHOD(SDL_Pixels, offsetGet)
{
	struct php_sdl_pixels *intern;
	long offset;

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}
	if (offset < 0 || offset >= (intern->pixels.h * intern->pixels.pitch)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid offset in SDL_Pixels", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	RETVAL_LONG(intern->pixels.pixels[offset]);
}
/* }}} */

/* {{{ proto SDL_Pixels, offsetUnset(int offset) */
PHP_METHOD(SDL_Pixels, offsetUnset)
{
	struct php_sdl_pixels *intern;
	long offset;

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &offset) == FAILURE) {
		return;
	}
	if (offset < 0 || offset >= (intern->pixels.h * intern->pixels.pitch)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid offset in SDL_Pixels", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	intern->pixels.pixels[offset] = 0;
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Pixels_offsetSet, 0, 0, 2)
       ZEND_ARG_INFO(0, offset)
       ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

/* {{{ proto SDL_Pixels, offsetSet(int offset, int value) */
PHP_METHOD(SDL_Pixels, offsetSet)
{
	struct php_sdl_pixels *intern;
	long offset, value;

	intern = (struct php_sdl_pixels *)Z_OBJ_P(getThis() TSRMLS_CC);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &offset, &value) == FAILURE) {
		return;
	}
	if (offset < 0 || offset >= (intern->pixels.h * intern->pixels.pitch)) {
		zend_throw_exception(zend_exception_get_default(TSRMLS_C), "Invalid offset in SDL_Pixels", 0 TSRMLS_CC);
		RETURN_FALSE;
	}
	intern->pixels.pixels[offset] = (Uint8)value;
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Pixels_GetByte, 0, 0, 2)
       ZEND_ARG_INFO(0, x)
       ZEND_ARG_INFO(0, y)
ZEND_END_ARG_INFO()

/* {{{ proto int SDL_Pixels::GetByte(int x, int y) */
PHP_METHOD(SDL_Pixels, GetByte)
{
	struct php_sdl_pixels *intern;
	zval *z_pixels;
	long x, y;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Oll", &z_pixels, php_sdl_pixels_ce, &x, &y) == FAILURE) {
		return;
	}
	intern = (struct php_sdl_pixels *)Z_OBJ_P(z_pixels TSRMLS_CC);\

	if (x < 0 || x >= intern->pixels.pitch || y < 0 || y >= intern->pixels.h) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid position (%ld,%ld) in SDL_Pixels (%d,%d)", x, y, intern->pixels.pitch, intern->pixels.h);
		RETURN_FALSE;
	}
	RETVAL_LONG(intern->pixels.pixels[y*intern->pixels.pitch+x]);
}
/* }}} */


ZEND_BEGIN_ARG_INFO_EX(arginfo_SDL_Pixels_SetByte, 0, 0, 3)
       ZEND_ARG_INFO(0, x)
       ZEND_ARG_INFO(0, y)
       ZEND_ARG_INFO(0, byte)
ZEND_END_ARG_INFO()

/* {{{ proto int SDL_Pixels::SetByte(int x, int y, int byte) */
PHP_METHOD(SDL_Pixels, SetByte)
{
	struct php_sdl_pixels *intern;
	zval *z_pixels;
	long x, y, v;

	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "Olll", &z_pixels, php_sdl_pixels_ce, &x, &y, &v) == FAILURE) {
		return;
	}
	intern = (struct php_sdl_pixels *)Z_OBJ_P(z_pixels TSRMLS_CC);\

	if (x < 0 || x >= intern->pixels.pitch || y < 0 || y >= intern->pixels.h) {
		php_error_docref(NULL TSRMLS_CC, E_NOTICE, "Invalid position (%ld,%ld) in SDL_Pixels (%d,%d)", x, y, intern->pixels.pitch, intern->pixels.h);
		RETURN_FALSE;
	}
	RETVAL_LONG(intern->pixels.pixels[y*intern->pixels.pitch+x]);
	intern->pixels.pixels[y*intern->pixels.pitch+x] = (Uint8)v;
}
/* }}} */



/* {{{ php_sdl_palette_free
	 */
static void php_sdl_palette_free(zend_object *object TSRMLS_DC)
{
	struct php_sdl_palette *intern = (struct php_sdl_palette *) object;

	if (intern->palette) {
		if (!(intern->flags & SDL_DONTFREE)) {
			SDL_FreePalette(intern->palette);
		}
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
}
/* }}} */

/* {{{ php_sdl_palette_new
 */
static zend_object* php_sdl_palette_new(zend_class_entry *class_type TSRMLS_DC)
{
	struct php_sdl_palette *intern;

	intern = ecalloc(1, sizeof(*intern));

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	intern->palette = NULL;
	intern->zo.handlers = (zend_object_handlers *) &php_sdl_palette_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ php_sdl_pixelformat_free
	 */
static void php_sdl_pixelformat_free(zend_object *object TSRMLS_DC)
{
	struct php_sdl_pixelformat *intern = (struct php_sdl_pixelformat *) object;

	if (intern->format) {
		if (!(intern->flags & SDL_DONTFREE)) {
			SDL_FreeFormat(intern->format);
		}
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
}
/* }}} */

/* {{{ php_sdl_pixelformat_new
 */
static zend_object* php_sdl_pixelformat_new(zend_class_entry *class_type TSRMLS_DC)
{
	struct php_sdl_pixelformat *intern;

	intern = ecalloc(1, sizeof(struct php_sdl_pixelformat) + zend_object_properties_size(class_type));

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	intern->format = NULL;
	intern->zo.handlers = (zend_object_handlers *) &php_sdl_pixelformat_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ php_sdl_pixels_free
	 */
static void php_sdl_pixels_free(zend_object *object TSRMLS_DC)
{
	struct php_sdl_pixels *intern = (struct php_sdl_pixels *) object;

	if (intern->pixels.pixels) {
		if (!(intern->flags & SDL_DONTFREE)) {
			efree(intern->pixels.pixels);
		}
	}

	zend_object_std_dtor(&intern->zo TSRMLS_CC);
}
/* }}} */

/* {{{ php_sdl_pixels_new
 */
static zend_object* php_sdl_pixels_new(zend_class_entry *class_type TSRMLS_DC)
{
	struct php_sdl_pixels *intern;

	intern = ecalloc(1, sizeof(*intern));

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	object_properties_init(&intern->zo, class_type);

	intern->zo.handlers = (zend_object_handlers *) &php_sdl_pixels_handlers;

	return &intern->zo;
}
/* }}} */

/* {{{ php_sdl_color_methods[] */
static const zend_function_entry php_sdl_color_methods[] = {
	PHP_ME(SDL_Color, __construct,     arginfo_SDL_Color__construct, ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Color, __toString,      arginfo_palette_none,         ZEND_ACC_PUBLIC)

	PHP_FE_END
};
/* }}} */

/* {{{ php_sdl_palette_methods[] */
static const zend_function_entry php_sdl_palette_methods[] = {
	PHP_ME(SDL_Palette, __construct,   arginfo_SDL_AllocPalette,        ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Palette, __toString,    arginfo_palette_none,            ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Palette, count,         arginfo_palette_none,            ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Palette, offsetExists,  arginfo_SDL_Palette_offset,      ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Palette, offsetGet,     arginfo_SDL_Palette_offset,      ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Palette, offsetSet,     arginfo_SDL_Palette_offsetSet,   ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Palette, offsetUnset,   arginfo_SDL_Palette_offset,      ZEND_ACC_PUBLIC)

	/* non-static methods */
	PHP_FALIAS(Free,             SDL_FreePalette,           arginfo_palette_none)
	PHP_FALIAS(SetColors,        SDL_SetPaletteColors,      arginfo_SDL_Palette_SetColors)

	PHP_FE_END
};
/* }}} */

/* {{{ php_sdl_pixelformat_methods[] */
static const zend_function_entry php_sdl_pixelformat_methods[] = {
	PHP_ME(SDL_PixelFormat, __construct,  arginfo_SDL_AllocFormat,            ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(SDL_PixelFormat, __toString,   arginfo_palette_none,               ZEND_ACC_PUBLIC)
	PHP_ME(SDL_PixelFormat, GetRGB,       arginfo_SDL_PixelFormat_GetRGB,     ZEND_ACC_PUBLIC)
	PHP_ME(SDL_PixelFormat, GetRGBA,      arginfo_SDL_PixelFormat_GetRGBA,    ZEND_ACC_PUBLIC)

	/* non-static methods */
	PHP_FALIAS(Free,             SDL_FreeFormat,             arginfo_format_none)
	PHP_FALIAS(SetPalette,       SDL_SetPixelFormatPalette,  arginfo_SDL_PixelFormat_SetPalette)
	PHP_FALIAS(MapRGB,           SDL_MapRGB,                 arginfo_SDL_PixelFormat_MapRGB)
	PHP_FALIAS(MapRGBA,          SDL_MapRGBA,                arginfo_SDL_PixelFormat_MapRGBA)

	PHP_FE_END
};
/* }}} */

/* {{{ php_sdl_pixels_methods[] */
static const zend_function_entry php_sdl_pixels_methods[] = {
	PHP_ME(SDL_Pixels,    __construct,   arginfo_SDL_Pixels__construct,     ZEND_ACC_CTOR|ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    __toString,    arginfo_format_none,               ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    count,         arginfo_format_none,               ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    offsetExists,  arginfo_SDL_Pixels_offset,         ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    offsetGet,     arginfo_SDL_Pixels_offset,         ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    offsetSet,     arginfo_SDL_Pixels_offsetSet,      ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    offsetUnset,   arginfo_SDL_Pixels_offset,         ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    GetByte,       arginfo_SDL_Pixels_GetByte,        ZEND_ACC_PUBLIC)
	PHP_ME(SDL_Pixels,    SetByte,       arginfo_SDL_Pixels_SetByte,        ZEND_ACC_PUBLIC)
	PHP_FE_END
};
/* }}} */

#define REGISTER_COLOR_PROP(name) \
	zend_declare_property_long(php_sdl_color_ce, ZEND_STRL(name), 0, ZEND_ACC_PUBLIC TSRMLS_CC)

#define REGISTER_PALETTE_PROP(name) \
	zend_declare_property_long(php_sdl_palette_ce, ZEND_STRL(name), 0, ZEND_ACC_PUBLIC TSRMLS_CC)

#define REGISTER_FORMAT_PROP(name) \
	zend_declare_property_long(php_sdl_pixelformat_ce, ZEND_STRL(name), 0, ZEND_ACC_PUBLIC TSRMLS_CC)

#define REGISTER_PIXELS_PROP(name) \
	zend_declare_property_long(php_sdl_pixels_ce, ZEND_STRL(name), 0, ZEND_ACC_PUBLIC TSRMLS_CC)

/* {{{ MINIT */
PHP_MINIT_FUNCTION(sdl_pixels)
{
	zend_class_entry ce;

	INIT_CLASS_ENTRY(ce, "SDL_Color", php_sdl_color_methods);
	php_sdl_color_ce = zend_register_internal_class(&ce TSRMLS_CC);
	memcpy(&php_sdl_color_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	REGISTER_COLOR_PROP("r");
	REGISTER_COLOR_PROP("g");
	REGISTER_COLOR_PROP("b");
	REGISTER_COLOR_PROP("a");

	INIT_CLASS_ENTRY(ce, "SDL_Palette", php_sdl_palette_methods);
	ce.create_object = php_sdl_palette_new;
	php_sdl_palette_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(php_sdl_palette_ce TSRMLS_CC, 1, zend_ce_arrayaccess);
	memcpy(&php_sdl_palette_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sdl_palette_handlers.free_obj = php_sdl_palette_free;

	REGISTER_PALETTE_PROP("ncolors");
	REGISTER_PALETTE_PROP("version");
	REGISTER_PALETTE_PROP("refcount");
	zend_declare_property_null(php_sdl_palette_ce, ZEND_STRL("colors"), ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "SDL_PixelFormat", php_sdl_pixelformat_methods);
	ce.create_object = php_sdl_pixelformat_new;
	php_sdl_pixelformat_ce = zend_register_internal_class(&ce TSRMLS_CC);
	memcpy(&php_sdl_pixelformat_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sdl_pixelformat_handlers.free_obj = php_sdl_pixelformat_free;

	REGISTER_FORMAT_PROP("format");
	REGISTER_FORMAT_PROP("BitsPerPixel");
	REGISTER_FORMAT_PROP("BytesPerPixel");
	REGISTER_FORMAT_PROP("Rmask");
	REGISTER_FORMAT_PROP("Gmask");
	REGISTER_FORMAT_PROP("Bmask");
	REGISTER_FORMAT_PROP("Amask");
	REGISTER_FORMAT_PROP("Rloss");
	REGISTER_FORMAT_PROP("Gloss");
	REGISTER_FORMAT_PROP("Bloss");
	REGISTER_FORMAT_PROP("Aloss");
	REGISTER_FORMAT_PROP("Rshift");
	REGISTER_FORMAT_PROP("Gshift");
	REGISTER_FORMAT_PROP("Bshift");
	REGISTER_FORMAT_PROP("Ashift");
	zend_declare_property_null(php_sdl_pixelformat_ce, ZEND_STRL("palette"), ZEND_ACC_PUBLIC TSRMLS_CC);

	INIT_CLASS_ENTRY(ce, "SDL_Pixels", php_sdl_pixels_methods);
	ce.create_object = php_sdl_pixels_new;
	php_sdl_pixels_ce = zend_register_internal_class(&ce TSRMLS_CC);
	zend_class_implements(php_sdl_pixels_ce TSRMLS_CC, 1, zend_ce_arrayaccess);
	memcpy(&php_sdl_pixels_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	php_sdl_pixels_handlers.free_obj = php_sdl_pixels_free;

	REGISTER_PIXELS_PROP("pitch");
	REGISTER_PIXELS_PROP("h");
	REGISTER_PIXELS_PROP("count");

	/* Pixel type. */
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_UNKNOWN",          SDL_PIXELTYPE_UNKNOWN,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_INDEX1",           SDL_PIXELTYPE_INDEX1,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_INDEX4",           SDL_PIXELTYPE_INDEX4,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_INDEX8",           SDL_PIXELTYPE_INDEX8,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_PACKED8",          SDL_PIXELTYPE_PACKED8,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_PACKED16",         SDL_PIXELTYPE_PACKED16,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_PACKED32",         SDL_PIXELTYPE_PACKED32,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_ARRAYU8",          SDL_PIXELTYPE_ARRAYU8,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_ARRAYU16",         SDL_PIXELTYPE_ARRAYU16,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_ARRAYU32",         SDL_PIXELTYPE_ARRAYU32,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_ARRAYF16",         SDL_PIXELTYPE_ARRAYF16,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELTYPE_ARRAYF32",         SDL_PIXELTYPE_ARRAYF32,           CONST_CS | CONST_PERSISTENT);

	/* Bitmap pixel order, high bit -> low bit. */
	REGISTER_LONG_CONSTANT("SDL_BITMAPORDER_NONE",           SDL_BITMAPORDER_NONE,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_BITMAPORDER_4321",           SDL_BITMAPORDER_4321,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_BITMAPORDER_1234",           SDL_BITMAPORDER_1234,             CONST_CS | CONST_PERSISTENT);

	/* Packed component order, high bit -> low bit. */
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_NONE",           SDL_PACKEDORDER_NONE,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_XRGB",           SDL_PACKEDORDER_XRGB,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_RGBX",           SDL_PACKEDORDER_RGBX,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_ARGB",           SDL_PACKEDORDER_ARGB,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_RGBA",           SDL_PACKEDORDER_RGBA,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_XBGR",           SDL_PACKEDORDER_XBGR,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_BGRX",           SDL_PACKEDORDER_BGRX,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_ABGR",           SDL_PACKEDORDER_ABGR,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDORDER_BGRA",           SDL_PACKEDORDER_BGRA,             CONST_CS | CONST_PERSISTENT);

	/* Packed component layout. */
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_NONE",          SDL_PACKEDLAYOUT_NONE,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_332",           SDL_PACKEDLAYOUT_332,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_4444",          SDL_PACKEDLAYOUT_4444,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_1555",          SDL_PACKEDLAYOUT_1555,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_5551",          SDL_PACKEDLAYOUT_5551,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_565",           SDL_PACKEDLAYOUT_565,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_8888",          SDL_PACKEDLAYOUT_8888,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_2101010",       SDL_PACKEDLAYOUT_2101010,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PACKEDLAYOUT_1010102",       SDL_PACKEDLAYOUT_1010102,         CONST_CS | CONST_PERSISTENT);

	/* Pixel format */
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_UNKNOWN",        SDL_PIXELFORMAT_UNKNOWN,          CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_INDEX1LSB",      SDL_PIXELFORMAT_INDEX1LSB,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_INDEX1MSB",      SDL_PIXELFORMAT_INDEX1MSB,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_INDEX4LSB",      SDL_PIXELFORMAT_INDEX4LSB,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_INDEX4MSB",      SDL_PIXELFORMAT_INDEX4MSB,        CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_INDEX8",         SDL_PIXELFORMAT_INDEX8,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGB332",         SDL_PIXELFORMAT_RGB332,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGB444",         SDL_PIXELFORMAT_RGB444,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGB555",         SDL_PIXELFORMAT_RGB555,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGR555",         SDL_PIXELFORMAT_BGR555,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ARGB4444",       SDL_PIXELFORMAT_ARGB4444,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGBA4444",       SDL_PIXELFORMAT_RGBA4444,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ABGR4444",       SDL_PIXELFORMAT_ABGR4444,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGRA4444",       SDL_PIXELFORMAT_BGRA4444,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ARGB1555",       SDL_PIXELFORMAT_ARGB1555,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGBA5551",       SDL_PIXELFORMAT_RGBA5551,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ABGR1555",       SDL_PIXELFORMAT_ABGR1555,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGRA5551",       SDL_PIXELFORMAT_BGRA5551,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGB565",         SDL_PIXELFORMAT_RGB565,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGR565",         SDL_PIXELFORMAT_BGR565,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGB24",          SDL_PIXELFORMAT_RGB24,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGR24",          SDL_PIXELFORMAT_BGR24,            CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGB888",         SDL_PIXELFORMAT_RGB888,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGBX8888",       SDL_PIXELFORMAT_RGBX8888,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGR888",         SDL_PIXELFORMAT_BGR888,           CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGRX8888",       SDL_PIXELFORMAT_BGRX8888,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ARGB8888",       SDL_PIXELFORMAT_ARGB8888,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_RGBA8888",       SDL_PIXELFORMAT_RGBA8888,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ABGR8888",       SDL_PIXELFORMAT_ABGR8888,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_BGRA8888",       SDL_PIXELFORMAT_BGRA8888,         CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_ARGB2101010",    SDL_PIXELFORMAT_ARGB2101010,      CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_YV12",           SDL_PIXELFORMAT_YV12,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_IYUV",           SDL_PIXELFORMAT_IYUV,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_YUY2",           SDL_PIXELFORMAT_YUY2,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_UYVY",           SDL_PIXELFORMAT_UYVY,             CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("SDL_PIXELFORMAT_YVYU",           SDL_PIXELFORMAT_YVYU,             CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */
