/*
 * This file was generated automatically by ExtUtils::ParseXS version 3.24 from the
 * contents of Collate.xs. Do not edit this file, edit Collate.xs instead.
 *
 *    ANY CHANGES MADE HERE WILL BE LOST!
 *
 */

#line 1 "Collate.xs"

#define PERL_NO_GET_CONTEXT /* we want efficiency */

/* I guese no private function needs pTHX_ and aTHX_ */

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

/* This file is prepared by mkheader */
#include "ucatbl.h"

/* Perl 5.6.1 ? */
#ifndef utf8n_to_uvuni
#define utf8n_to_uvuni  utf8_to_uv
#endif /* utf8n_to_uvuni */

/* UTF8_ALLOW_BOM is used before Perl 5.8.0 */
#ifndef UTF8_ALLOW_BOM
#define UTF8_ALLOW_BOM  (0)
#endif /* UTF8_ALLOW_BOM */

#ifndef UTF8_ALLOW_SURROGATE
#define UTF8_ALLOW_SURROGATE  (0)
#endif /* UTF8_ALLOW_SURROGATE */

#ifndef UTF8_ALLOW_FE_FF
#define UTF8_ALLOW_FE_FF  (0)
#endif /* UTF8_ALLOW_FE_FF */

#ifndef UTF8_ALLOW_FFFF
#define UTF8_ALLOW_FFFF  (0)
#endif /* UTF8_ALLOW_FFFF */

#define AllowAnyUTF (UTF8_ALLOW_SURROGATE|UTF8_ALLOW_BOM|UTF8_ALLOW_FE_FF|UTF8_ALLOW_FFFF)

/* perl 5.6.x workaround, before 5.8.0 */
#ifdef utf8n_to_uvuni
#define GET_UV_FOR_5_6	utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF)
#else
#define GET_UV_FOR_5_6	retlen = 1 /* avoid an infinite loop */
#endif /* utf8n_to_uvuni */

/* At present, char > 0x10ffff are unaffected without complaint, right? */
#define VALID_UTF_MAX    (0x10ffff)
#define OVER_UTF_MAX(uv) (VALID_UTF_MAX < (uv))

static const UV max_div_16 = UV_MAX / 16;

/* Supported Levels */
#define MinLevel	(1)
#define MaxLevel	(4)

/* Shifted weight at 4th level */
#define Shift4Wt	(0xFFFF)

#define VCE_Length	(9)

#define Hangul_SBase  (0xAC00)
#define Hangul_SIni   (0xAC00)
#define Hangul_SFin   (0xD7A3)
#define Hangul_NCount (588)
#define Hangul_TCount (28)
#define Hangul_LBase  (0x1100)
#define Hangul_LIni   (0x1100)
#define Hangul_LFin   (0x1159)
#define Hangul_LFill  (0x115F)
#define Hangul_LEnd   (0x115F) /* Unicode 5.2 */
#define Hangul_VBase  (0x1161)
#define Hangul_VIni   (0x1160) /* from Vowel Filler */
#define Hangul_VFin   (0x11A2)
#define Hangul_VEnd   (0x11A7) /* Unicode 5.2 */
#define Hangul_TBase  (0x11A7) /* from "no-final" codepoint */
#define Hangul_TIni   (0x11A8)
#define Hangul_TFin   (0x11F9)
#define Hangul_TEnd   (0x11FF) /* Unicode 5.2 */
#define HangulL2Ini   (0xA960) /* Unicode 5.2 */
#define HangulL2Fin   (0xA97C) /* Unicode 5.2 */
#define HangulV2Ini   (0xD7B0) /* Unicode 5.2 */
#define HangulV2Fin   (0xD7C6) /* Unicode 5.2 */
#define HangulT2Ini   (0xD7CB) /* Unicode 5.2 */
#define HangulT2Fin   (0xD7FB) /* Unicode 5.2 */

#define CJK_UidIni    (0x4E00)
#define CJK_UidFin    (0x9FA5)
#define CJK_UidF41    (0x9FBB)
#define CJK_UidF51    (0x9FC3)
#define CJK_UidF52    (0x9FCB)
#define CJK_UidF61    (0x9FCC)
#define CJK_ExtAIni   (0x3400) /* Unicode 3.0 */
#define CJK_ExtAFin   (0x4DB5) /* Unicode 3.0 */
#define CJK_ExtBIni  (0x20000) /* Unicode 3.1 */
#define CJK_ExtBFin  (0x2A6D6) /* Unicode 3.1 */
#define CJK_ExtCIni  (0x2A700) /* Unicode 5.2 */
#define CJK_ExtCFin  (0x2B734) /* Unicode 5.2 */
#define CJK_ExtDIni  (0x2B740) /* Unicode 6.0 */
#define CJK_ExtDFin  (0x2B81D) /* Unicode 6.0 */

#define CJK_CompIni  (0xFA0E)
#define CJK_CompFin  (0xFA29)
static STDCHAR UnifiedCompat[] = {
      1,1,0,1,0,1,1,0,0,0,0,0,0,0,0,0,0,1,0,1,0,1,1,0,0,1,1,1
}; /* E F 0 1 2 3 4 5 6 7 8 9 A B C D E F 0 1 2 3 4 5 6 7 8 9 */

#define codeRange(bcode, ecode)	((bcode) <= code && code <= (ecode))

#line 117 "Collate.c"
#ifndef PERL_UNUSED_VAR
#  define PERL_UNUSED_VAR(var) if (0) var = var
#endif

#ifndef dVAR
#  define dVAR		dNOOP
#endif


/* This stuff is not part of the API! You have been warned. */
#ifndef PERL_VERSION_DECIMAL
#  define PERL_VERSION_DECIMAL(r,v,s) (r*1000000 + v*1000 + s)
#endif
#ifndef PERL_DECIMAL_VERSION
#  define PERL_DECIMAL_VERSION \
	  PERL_VERSION_DECIMAL(PERL_REVISION,PERL_VERSION,PERL_SUBVERSION)
#endif
#ifndef PERL_VERSION_GE
#  define PERL_VERSION_GE(r,v,s) \
	  (PERL_DECIMAL_VERSION >= PERL_VERSION_DECIMAL(r,v,s))
#endif
#ifndef PERL_VERSION_LE
#  define PERL_VERSION_LE(r,v,s) \
	  (PERL_DECIMAL_VERSION <= PERL_VERSION_DECIMAL(r,v,s))
#endif

/* XS_INTERNAL is the explicit static-linkage variant of the default
 * XS macro.
 *
 * XS_EXTERNAL is the same as XS_INTERNAL except it does not include
 * "STATIC", ie. it exports XSUB symbols. You probably don't want that
 * for anything but the BOOT XSUB.
 *
 * See XSUB.h in core!
 */


/* TODO: This might be compatible further back than 5.10.0. */
#if PERL_VERSION_GE(5, 10, 0) && PERL_VERSION_LE(5, 15, 1)
#  undef XS_EXTERNAL
#  undef XS_INTERNAL
#  if defined(__CYGWIN__) && defined(USE_DYNAMIC_LOADING)
#    define XS_EXTERNAL(name) __declspec(dllexport) XSPROTO(name)
#    define XS_INTERNAL(name) STATIC XSPROTO(name)
#  endif
#  if defined(__SYMBIAN32__)
#    define XS_EXTERNAL(name) EXPORT_C XSPROTO(name)
#    define XS_INTERNAL(name) EXPORT_C STATIC XSPROTO(name)
#  endif
#  ifndef XS_EXTERNAL
#    if defined(HASATTRIBUTE_UNUSED) && !defined(__cplusplus)
#      define XS_EXTERNAL(name) void name(pTHX_ CV* cv __attribute__unused__)
#      define XS_INTERNAL(name) STATIC void name(pTHX_ CV* cv __attribute__unused__)
#    else
#      ifdef __cplusplus
#        define XS_EXTERNAL(name) extern "C" XSPROTO(name)
#        define XS_INTERNAL(name) static XSPROTO(name)
#      else
#        define XS_EXTERNAL(name) XSPROTO(name)
#        define XS_INTERNAL(name) STATIC XSPROTO(name)
#      endif
#    endif
#  endif
#endif

/* perl >= 5.10.0 && perl <= 5.15.1 */


/* The XS_EXTERNAL macro is used for functions that must not be static
 * like the boot XSUB of a module. If perl didn't have an XS_EXTERNAL
 * macro defined, the best we can do is assume XS is the same.
 * Dito for XS_INTERNAL.
 */
#ifndef XS_EXTERNAL
#  define XS_EXTERNAL(name) XS(name)
#endif
#ifndef XS_INTERNAL
#  define XS_INTERNAL(name) XS(name)
#endif

/* Now, finally, after all this mess, we want an ExtUtils::ParseXS
 * internal macro that we're free to redefine for varying linkage due
 * to the EXPORT_XSUB_SYMBOLS XS keyword. This is internal, use
 * XS_EXTERNAL(name) or XS_INTERNAL(name) in your code if you need to!
 */

#undef XS_EUPXS
#if defined(PERL_EUPXS_ALWAYS_EXPORT)
#  define XS_EUPXS(name) XS_EXTERNAL(name)
#else
   /* default to internal */
#  define XS_EUPXS(name) XS_INTERNAL(name)
#endif

#ifndef PERL_ARGS_ASSERT_CROAK_XS_USAGE
#define PERL_ARGS_ASSERT_CROAK_XS_USAGE assert(cv); assert(params)

/* prototype to pass -Wmissing-prototypes */
STATIC void
S_croak_xs_usage(pTHX_ const CV *const cv, const char *const params);

STATIC void
S_croak_xs_usage(pTHX_ const CV *const cv, const char *const params)
{
    const GV *const gv = CvGV(cv);

    PERL_ARGS_ASSERT_CROAK_XS_USAGE;

    if (gv) {
        const char *const gvname = GvNAME(gv);
        const HV *const stash = GvSTASH(gv);
        const char *const hvname = stash ? HvNAME(stash) : NULL;

        if (hvname)
            Perl_croak(aTHX_ "Usage: %s::%s(%s)", hvname, gvname, params);
        else
            Perl_croak(aTHX_ "Usage: %s(%s)", gvname, params);
    } else {
        /* Pants. I don't think that it should be possible to get here. */
        Perl_croak(aTHX_ "Usage: CODE(0x%"UVxf")(%s)", PTR2UV(cv), params);
    }
}
#undef  PERL_ARGS_ASSERT_CROAK_XS_USAGE

#ifdef PERL_IMPLICIT_CONTEXT
#define croak_xs_usage(a,b)    S_croak_xs_usage(aTHX_ a,b)
#else
#define croak_xs_usage        S_croak_xs_usage
#endif

#endif

/* NOTE: the prototype of newXSproto() is different in versions of perls,
 * so we define a portable version of newXSproto()
 */
#ifdef newXS_flags
#define newXSproto_portable(name, c_impl, file, proto) newXS_flags(name, c_impl, file, proto, 0)
#else
#define newXSproto_portable(name, c_impl, file, proto) (PL_Sv=(SV*)newXS(name, c_impl, file), sv_setpv(PL_Sv, proto), (CV*)PL_Sv)
#endif /* !defined(newXS_flags) */

#line 259 "Collate.c"

XS_EUPXS(XS_Unicode__Collate__fetch_rest); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__fetch_rest)
{
    dVAR; dXSARGS;
    if (items != 0)
       croak_xs_usage(cv,  "");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
#line 114 "Collate.xs"
    char ** rest;
#line 272 "Collate.c"
#line 116 "Collate.xs"
    for (rest = UCA_rest; *rest; ++rest) {
	XPUSHs(sv_2mortal(newSVpv((char *) *rest, 0)));
    }
#line 277 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate__fetch_simple); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__fetch_simple)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	UV	uv = (UV)SvUV(ST(0))
;
#line 125 "Collate.xs"
    U8 ***plane, **row;
    U8* result = NULL;
#line 298 "Collate.c"
#line 128 "Collate.xs"
    if (!OVER_UTF_MAX(uv)){
	plane = (U8***)UCA_simple[uv >> 16];
	if (plane) {
	    row = plane[(uv >> 8) & 0xff];
	    result = row ? row[uv & 0xff] : NULL;
	}
    }
    if (result) {
	int i;
	int num = (int)*result;
	++result;
	for (i = 0; i < num; ++i) {
	    XPUSHs(sv_2mortal(newSVpvn((char *) result, VCE_Length)));
	    result += VCE_Length;
	}
    } else {
	XPUSHs(sv_2mortal(newSViv(0)));
    }
#line 318 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate__ignorable_simple); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__ignorable_simple)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
#line 153 "Collate.xs"
    U8 ***plane, **row;
    int num = -1;
    U8* result = NULL;
#line 339 "Collate.c"
	SV *	RETVAL;
#line 157 "Collate.xs"
    if (!OVER_UTF_MAX(uv)){
	plane = (U8***)UCA_simple[uv >> 16];
	if (plane) {
	    row = plane[(uv >> 8) & 0xff];
	    result = row ? row[uv & 0xff] : NULL;
	}
	if (result)
	    num = (int)*result; /* assuming 0 <= num < 128 */
    }

    if (ix)
	RETVAL = boolSV(num >0);
    else
	RETVAL = boolSV(num==0);
#line 356 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate__getHexArray); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__getHexArray)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	SV*	src = ST(0)
;
#line 179 "Collate.xs"
    char *s, *e;
    STRLEN byte;
    UV value;
    bool overflowed = FALSE;
    const char *hexdigit;
#line 381 "Collate.c"
#line 185 "Collate.xs"
    s = SvPV(src,byte);
    for (e = s + byte; s < e;) {
	hexdigit = strchr((char *) PL_hexdigit, *s++);
        if (! hexdigit)
	    continue;
	value = (hexdigit - PL_hexdigit) & 0xF;
	while (*s) {
	    hexdigit = strchr((char *) PL_hexdigit, *s++);
	    if (! hexdigit)
		break;
	    if (overflowed)
		continue;
	    if (value > max_div_16) {
		overflowed = TRUE;
		continue;
	    }
	    value = (value << 4) | ((hexdigit - PL_hexdigit) & 0xF);
	}
	XPUSHs(sv_2mortal(newSVuv(overflowed ? UV_MAX : value)));
    }
#line 403 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate__isIllegal); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__isIllegal)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "sv");
    {
	SV*	sv = ST(0)
;
#line 211 "Collate.xs"
    UV uv;
#line 421 "Collate.c"
	SV *	RETVAL;
#line 213 "Collate.xs"
    if (!sv || !SvIOK(sv))
	XSRETURN_YES;
    uv = SvUVX(sv);
    RETVAL = boolSV(
	   0x10FFFF < uv                   /* out of range */
	|| ((uv & 0xFFFE) == 0xFFFE)       /* ??FFF[EF] */
	|| (0xD800 <= uv && uv <= 0xDFFF)  /* unpaired surrogates */
	|| (0xFDD0 <= uv && uv <= 0xFDEF)  /* other non-characters */
    );
#line 433 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate__decompHangul); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__decompHangul)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "code");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	UV	code = (UV)SvUV(ST(0))
;
#line 230 "Collate.xs"
    UV sindex, lindex, vindex, tindex;
#line 454 "Collate.c"
#line 232 "Collate.xs"
    /* code *must* be in Hangul syllable.
     * Check it before you enter here. */
    sindex =  code - Hangul_SBase;
    lindex =  sindex / Hangul_NCount;
    vindex = (sindex % Hangul_NCount) / Hangul_TCount;
    tindex =  sindex % Hangul_TCount;

    XPUSHs(sv_2mortal(newSVuv(lindex + Hangul_LBase)));
    XPUSHs(sv_2mortal(newSVuv(vindex + Hangul_VBase)));
    if (tindex)
	XPUSHs(sv_2mortal(newSVuv(tindex + Hangul_TBase)));
#line 467 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate_getHST); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate_getHST)
{
    dVAR; dXSARGS;
    if (items < 1 || items > 2)
       croak_xs_usage(cv,  "code, uca_vers = 0");
    {
	UV	code = (UV)SvUV(ST(0))
;
	IV	uca_vers;
#line 250 "Collate.xs"
    const char * hangtype;
    STRLEN typelen;
#line 487 "Collate.c"
	SV *	RETVAL;

	if (items < 2)
	    uca_vers = 0;
	else {
	    uca_vers = (IV)SvIV(ST(1))
;
	}
#line 253 "Collate.xs"
    if (codeRange(Hangul_SIni, Hangul_SFin)) {
	if ((code - Hangul_SBase) % Hangul_TCount) {
	    hangtype = "LVT"; typelen = 3;
	} else {
	    hangtype = "LV"; typelen = 2;
	}
    } else if (uca_vers < 20) {
	if (codeRange(Hangul_LIni, Hangul_LFin) || code == Hangul_LFill) {
	    hangtype = "L"; typelen = 1;
	} else if (codeRange(Hangul_VIni, Hangul_VFin)) {
	    hangtype = "V"; typelen = 1;
	} else if (codeRange(Hangul_TIni, Hangul_TFin)) {
	    hangtype = "T"; typelen = 1;
	} else {
	    hangtype = ""; typelen = 0;
	}
    } else {
	if        (codeRange(Hangul_LIni, Hangul_LEnd) ||
		   codeRange(HangulL2Ini, HangulL2Fin)) {
	    hangtype = "L"; typelen = 1;
	} else if (codeRange(Hangul_VIni, Hangul_VEnd) ||
		   codeRange(HangulV2Ini, HangulV2Fin)) {
	    hangtype = "V"; typelen = 1;
	} else if (codeRange(Hangul_TIni, Hangul_TEnd) ||
		   codeRange(HangulT2Ini, HangulT2Fin)) {
	    hangtype = "T"; typelen = 1;
	} else {
	    hangtype = ""; typelen = 0;
	}
    }

    RETVAL = newSVpvn(hangtype, typelen);
#line 529 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate__derivCE_9); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__derivCE_9)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "code");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	UV	code = (UV)SvUV(ST(0))
;
#line 299 "Collate.xs"
    UV base, aaaa, bbbb;
    U8 a[VCE_Length + 1] = "\x00\xFF\xFF\x00\x20\x00\x02\xFF\xFF";
    U8 b[VCE_Length + 1] = "\x00\xFF\xFF\x00\x00\x00\x00\xFF\xFF";
    bool basic_unified = 0;
#line 554 "Collate.c"
#line 304 "Collate.xs"
    if (CJK_UidIni <= code) {
	if (codeRange(CJK_CompIni, CJK_CompFin))
	    basic_unified = (bool)UnifiedCompat[code - CJK_CompIni];
	else
	    basic_unified = (ix >= 5 ? (code <= CJK_UidF61) :
			     ix >= 3 ? (code <= CJK_UidF52) :
			     ix == 2 ? (code <= CJK_UidF51) :
			     ix == 1 ? (code <= CJK_UidF41) :
				       (code <= CJK_UidFin));
    }
    base = (basic_unified)
	    ? 0xFB40 : /* CJK */
	   ((codeRange(CJK_ExtAIni, CJK_ExtAFin))
		||
	    (codeRange(CJK_ExtBIni, CJK_ExtBFin))
		||
	    (ix >= 3 && codeRange(CJK_ExtCIni, CJK_ExtCFin))
		||
	    (ix >= 4 && codeRange(CJK_ExtDIni, CJK_ExtDFin)))
	    ? 0xFB80   /* CJK ext. */
	    : 0xFBC0;  /* others */
    aaaa =  base + (code >> 15);
    bbbb = (code & 0x7FFF) | 0x8000;
    a[1] = (U8)(aaaa >> 8);
    a[2] = (U8)(aaaa & 0xFF);
    b[1] = (U8)(bbbb >> 8);
    b[2] = (U8)(bbbb & 0xFF);
    a[7] = b[7] = (U8)(code >> 8);
    a[8] = b[8] = (U8)(code & 0xFF);
    XPUSHs(sv_2mortal(newSVpvn((char *) a, VCE_Length)));
    XPUSHs(sv_2mortal(newSVpvn((char *) b, VCE_Length)));
#line 587 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate__derivCE_8); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__derivCE_8)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "code");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	UV	code = (UV)SvUV(ST(0))
;
#line 341 "Collate.xs"
    UV aaaa, bbbb;
    U8 a[VCE_Length + 1] = "\x00\xFF\xFF\x00\x02\x00\x01\xFF\xFF";
    U8 b[VCE_Length + 1] = "\x00\xFF\xFF\x00\x00\x00\x00\xFF\xFF";
#line 609 "Collate.c"
#line 345 "Collate.xs"
    aaaa =  0xFF80 + (code >> 15);
    bbbb = (code & 0x7FFF) | 0x8000;
    a[1] = (U8)(aaaa >> 8);
    a[2] = (U8)(aaaa & 0xFF);
    b[1] = (U8)(bbbb >> 8);
    b[2] = (U8)(bbbb & 0xFF);
    a[7] = b[7] = (U8)(code >> 8);
    a[8] = b[8] = (U8)(code & 0xFF);
    XPUSHs(sv_2mortal(newSVpvn((char *) a, VCE_Length)));
    XPUSHs(sv_2mortal(newSVpvn((char *) b, VCE_Length)));
#line 621 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate__uideoCE_8); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__uideoCE_8)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "code");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	UV	code = (UV)SvUV(ST(0))
;
#line 361 "Collate.xs"
    U8 uice[VCE_Length + 1] = "\x00\xFF\xFF\x00\x20\x00\x02\xFF\xFF";
#line 641 "Collate.c"
#line 363 "Collate.xs"
    uice[1] = uice[7] = (U8)(code >> 8);
    uice[2] = uice[8] = (U8)(code & 0xFF);
    XPUSHs(sv_2mortal(newSVpvn((char *) uice, VCE_Length)));
#line 646 "Collate.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Unicode__Collate__isUIdeo); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate__isUIdeo)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "code, uca_vers");
    {
	UV	code = (UV)SvUV(ST(0))
;
	IV	uca_vers = (IV)SvIV(ST(1))
;
	bool	basic_unified = 0;
	SV *	RETVAL;
#line 374 "Collate.xs"
    /* uca_vers = 0 for _uideoCE_8() */
    if (CJK_UidIni <= code) {
	if (codeRange(CJK_CompIni, CJK_CompFin))
	    basic_unified = (bool)UnifiedCompat[code - CJK_CompIni];
	else
	    basic_unified = (uca_vers >= 24 ? (code <= CJK_UidF61) :
			     uca_vers >= 20 ? (code <= CJK_UidF52) :
			     uca_vers >= 18 ? (code <= CJK_UidF51) :
			     uca_vers >= 14 ? (code <= CJK_UidF41) :
					      (code <= CJK_UidFin));
    }
    RETVAL = boolSV(
	(basic_unified)
		||
	(codeRange(CJK_ExtAIni, CJK_ExtAFin))
		||
	(uca_vers >=  8 && codeRange(CJK_ExtBIni, CJK_ExtBFin))
		||
	(uca_vers >= 20 && codeRange(CJK_ExtCIni, CJK_ExtCFin))
		||
	(uca_vers >= 22 && codeRange(CJK_ExtDIni, CJK_ExtDFin))
    );
#line 689 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate_mk_SortKey); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate_mk_SortKey)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "self, buf");
    {
	SV*	self = ST(0)
;
	SV*	buf = ST(1)
;
#line 405 "Collate.xs"
    SV *dst, **svp;
    STRLEN dlen, vlen;
    U8 *d, *p, *e, *v, *s[MaxLevel], *eachlevel[MaxLevel];
    AV *bufAV;
    HV *selfHV;
    UV back_flag;
    I32 i, buf_len;
    IV  lv, level, uca_vers;
    bool upper_lower, kata_hira, v2i, last_is_var;
#line 718 "Collate.c"
	SV *	RETVAL;
#line 415 "Collate.xs"
    if (SvROK(self) && SvTYPE(SvRV(self)) == SVt_PVHV)
	selfHV = (HV*)SvRV(self);
    else
	croak("$self is not a HASHREF.");

    if (SvROK(buf) && SvTYPE(SvRV(buf)) == SVt_PVAV)
	bufAV = (AV*)SvRV(buf);
    else
	croak("XSUB, not an ARRAYREF.");

    buf_len = av_len(bufAV);

    if (buf_len < 0) { /* empty: -1 */
	dlen = 2 * (MaxLevel - 1);
	dst = newSV(dlen);
	(void)SvPOK_only(dst);
	d = (U8*)SvPVX(dst);
	while (dlen--)
	    *d++ = '\0';
    } else {
	svp = hv_fetch(selfHV, "level", 5, FALSE);
	level = svp ? SvIV(*svp) : MaxLevel;

	for (lv = 0; lv < level; lv++) {
	    New(0, eachlevel[lv], 2 * (1 + buf_len) + 1, U8);
	    s[lv] = eachlevel[lv];
	}

	svp = hv_fetch(selfHV, "upper_before_lower", 18, FALSE);
	upper_lower = svp ? SvTRUE(*svp) : FALSE;
	svp = hv_fetch(selfHV, "katakana_before_hiragana", 24, FALSE);
	kata_hira = svp ? SvTRUE(*svp) : FALSE;
	svp = hv_fetch(selfHV, "UCA_Version", 11, FALSE);
	uca_vers = SvIV(*svp);
	svp = hv_fetch(selfHV, "variable", 8, FALSE);
	v2i = uca_vers >= 9 && svp /* (vers >= 9) and not (non-ignorable) */
	    ? !(SvCUR(*svp) == 13 && memEQ(SvPVX(*svp), "non-ignorable", 13))
	    : FALSE;

	last_is_var = FALSE;
	for (i = 0; i <= buf_len; i++) {
	    svp = av_fetch(bufAV, i, FALSE);

	    if (svp && SvPOK(*svp))
		v = (U8*)SvPV(*svp, vlen);
	    else
		croak("not a vwt.");

	    if (vlen < VCE_Length) /* ignore short VCE (unexpected) */
		continue;

	    /* "Ignorable (L1, L2) after Variable" since track. v. 9 */
	    if (v2i) {
		if (*v)
		    last_is_var = TRUE;
		else if (v[1] || v[2]) /* non zero primary weight */
		    last_is_var = FALSE;
		else if (last_is_var) /* zero primary weight; skipped */
		    continue;
	    }

	    if (v[5] == 0) { /* tert wt < 256 */
		if (upper_lower) {
		    if (0x8 <= v[6] && v[6] <= 0xC) /* lower */
			v[6] -= 6;
		    else if (0x2 <= v[6] && v[6] <= 0x6) /* upper */
			v[6] += 6;
		    else if (v[6] == 0x1C) /* square upper */
			v[6]++;
		    else if (v[6] == 0x1D) /* square lower */
			v[6]--;
		}
		if (kata_hira) {
		    if (0x0F <= v[6] && v[6] <= 0x13) /* katakana */
			v[6] -= 2;
		    else if (0xD <= v[6] && v[6] <= 0xE) /* hiragana */
			v[6] += 5;
		}
	    }

	    for (lv = 0; lv < level; lv++) {
		if (v[2 * lv + 1] || v[2 * lv + 2]) {
		    *s[lv]++ = v[2 * lv + 1];
		    *s[lv]++ = v[2 * lv + 2];
		}
	    }
	}

	dlen = 2 * (MaxLevel - 1);
	for (lv = 0; lv < level; lv++)
	    dlen += s[lv] - eachlevel[lv];

	dst = newSV(dlen);
	(void)SvPOK_only(dst);
	d = (U8*)SvPVX(dst);

	svp = hv_fetch(selfHV, "backwardsFlag", 13, FALSE);
	back_flag = svp ? SvUV(*svp) : (UV)0;

	for (lv = 0; lv < level; lv++) {
	    if (back_flag & (1 << (lv + 1))) {
		p = s[lv];
		e = eachlevel[lv];
		for ( ; e < p; p -= 2) {
		    *d++ = p[-2];
		    *d++ = p[-1];
		}
	    }
	    else {
		p = eachlevel[lv];
		e = s[lv];
		while (p < e)
		    *d++ = *p++;
	    }
	    if (lv + 1 < MaxLevel) { /* lv + 1 == real level */
		*d++ = '\0';
		*d++ = '\0';
	    }
	}

	for (lv = level; lv < MaxLevel; lv++) {
	    if (lv + 1 < MaxLevel) { /* lv + 1 == real level */
		*d++ = '\0';
		*d++ = '\0';
	    }
	}

	for (lv = 0; lv < level; lv++) {
	    Safefree(eachlevel[lv]);
	}
    }
    *d = '\0';
    SvCUR_set(dst, d - (U8*)SvPVX(dst));
    RETVAL = dst;
#line 855 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate_varCE); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate_varCE)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "self, vce");
    {
	SV*	self = ST(0)
;
	SV*	vce = ST(1)
;
#line 558 "Collate.xs"
    SV *dst, *vbl, **svp;
    HV *selfHV;
    U8 *a, *v, *d;
    STRLEN alen, vlen;
    bool ig_l2;
    UV totwt;
#line 881 "Collate.c"
	SV *	RETVAL;
#line 565 "Collate.xs"
    if (SvROK(self) && SvTYPE(SvRV(self)) == SVt_PVHV)
	selfHV = (HV*)SvRV(self);
    else
	croak("$self is not a HASHREF.");

    svp = hv_fetch(selfHV, "ignore_level2", 13, FALSE);
    ig_l2 = svp ? SvTRUE(*svp) : FALSE;

    svp = hv_fetch(selfHV, "variable", 8, FALSE);
    vbl = svp ? *svp : &PL_sv_no;
    a = (U8*)SvPV(vbl, alen);
    v = (U8*)SvPV(vce, vlen);

    dst = newSV(vlen);
    d = (U8*)SvPVX(dst);
    (void)SvPOK_only(dst);
    Copy(v, d, vlen, U8);
    SvCUR_set(dst, vlen);
    d[vlen] = '\0';

    /* primary weight == 0 && secondary weight != 0 */
    if (ig_l2 && !d[1] && !d[2] && (d[3] || d[4])) {
	d[3] = d[4] = d[5] = d[6] = '\0';
    }

    /* variable: checked only the first char and the length,
       trusting checkCollator() and %VariableOK in Perl ... */

    if (vlen < VCE_Length /* ignore short VCE (unexpected) */
	||
	*a == 'n') /* non-ignorable */
	1;
    else if (*v) {
	if (*a == 's') { /* shifted or shift-trimmed */
	    d[7] = d[1]; /* wt level 1 to 4 */
	    d[8] = d[2];
	} /* else blanked */

	d[1] = d[2] = d[3] = d[4] = d[5] = d[6] = '\0';
    }
    else if (*a == 'b') /* blanked */
	1;
    else if (*a == 's') { /* shifted or shift-trimmed */
	totwt = d[1] + d[2] + d[3] + d[4] + d[5] + d[6];
	if (alen == 7 && totwt != 0) { /* shifted */
	    if (d[1] == 0 && d[2] == 1) { /* XXX: CollationAuxiliary-6.2.0 */
		d[7] = d[1]; /* wt level 1 to 4 */
		d[8] = d[2];
	    } else {
		d[7] = (U8)(Shift4Wt >> 8);
		d[8] = (U8)(Shift4Wt & 0xFF);
	    }
	} else { /* shift-trimmed or completely ignorable */
	    d[7] = d[8] = '\0';
	}
    }
    else
	croak("unknown variable value '%s'", a);
    RETVAL = dst;
#line 943 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate_visualizeSortKey); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate_visualizeSortKey)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "self, key");
    {
	SV *	self = ST(0)
;
	SV *	key = ST(1)
;
#line 634 "Collate.xs"
    HV *selfHV;
    SV **svp, *dst;
    U8 *s, *e, *d;
    STRLEN klen, dlen;
    UV uv;
    IV uca_vers, sep = 0;
    static const char *upperhex = "0123456789ABCDEF";
#line 970 "Collate.c"
	SV *	RETVAL;
#line 642 "Collate.xs"
    if (SvROK(self) && SvTYPE(SvRV(self)) == SVt_PVHV)
	selfHV = (HV*)SvRV(self);
    else
	croak("$self is not a HASHREF.");

    svp = hv_fetch(selfHV, "UCA_Version", 11, FALSE);
    if (!svp)
	croak("Panic: no $self->{UCA_Version} in visualizeSortKey");
    uca_vers = SvIV(*svp);

    s = (U8*)SvPV(key, klen);

   /* slightly *longer* than the need, but I'm afraid of miscounting;
      = (klen / 2) * 5 - 1
       + 2   # '[' and ']'
       + 1   # '\0'
       (a) if klen is odd (not expected), maybe more 5 bytes.
       (b) there is not always the identical level.
   */
    dlen = (klen / 2) * 5 + MaxLevel * 2 + 2;
    dst = newSV(dlen);
    (void)SvPOK_only(dst);
    d = (U8*)SvPVX(dst);

    *d++ = '[';
    for (e = s + klen; s < e; s += 2) {
	uv = (U16)(*s << 8 | s[1]);
	if (uv || sep >= MaxLevel) {
	    if ((d[-1] != '[') && ((9 <= uca_vers) || (d[-1] != '|')))
		*d++ = ' ';
	    *d++ = upperhex[ (s[0] >> 4) & 0xF ];
	    *d++ = upperhex[  s[0]       & 0xF ];
	    *d++ = upperhex[ (s[1] >> 4) & 0xF ];
	    *d++ = upperhex[  s[1]       & 0xF ];
	} else {
	    if ((9 <= uca_vers) && (d[-1] != '['))
		*d++ = ' ';
	    *d++ = '|';
	    ++sep;
	}
    }
    *d++ = ']';
    *d   = '\0';
    SvCUR_set(dst, d - (U8*)SvPVX(dst));
    RETVAL = dst;
#line 1018 "Collate.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Collate_unpackUfor56); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Collate_unpackUfor56)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	SV*	src = ST(0)
;
#line 698 "Collate.xs"
    STRLEN srclen, retlen;
    U8 *s, *p, *e;
    UV uv;
#line 1041 "Collate.c"
#line 702 "Collate.xs"
    s = (U8*)SvPV(src,srclen);
    if (!SvUTF8(src)) {
	SV* tmpsv = sv_mortalcopy(src);
	if (!SvPOK(tmpsv))
	    (void)sv_pvn_force(tmpsv,&srclen);
	sv_utf8_upgrade(tmpsv);
	s = (U8*)SvPV(tmpsv,srclen);
    }
    e = s + srclen;

    for (p = s; p < e; p += retlen) {
	uv = GET_UV_FOR_5_6; /* perl 5.6.x workaround */
	if (!retlen)
	    croak("panic (Unicode::Collate): zero-length character");
	XPUSHs(sv_2mortal(newSVuv(uv)));
    }
#line 1059 "Collate.c"
	PUTBACK;
	return;
    }
}

#ifdef __cplusplus
extern "C"
#endif
XS_EXTERNAL(boot_Unicode__Collate); /* prototype to pass -Wmissing-prototypes */
XS_EXTERNAL(boot_Unicode__Collate)
{
    dVAR; dXSARGS;
#if (PERL_REVISION == 5 && PERL_VERSION < 9)
    char* file = __FILE__;
#else
    const char* file = __FILE__;
#endif

    PERL_UNUSED_VAR(cv); /* -W */
    PERL_UNUSED_VAR(items); /* -W */
#ifdef XS_APIVERSION_BOOTCHECK
    XS_APIVERSION_BOOTCHECK;
#endif
    XS_VERSION_BOOTCHECK;

        newXS("Unicode::Collate::_fetch_rest", XS_Unicode__Collate__fetch_rest, file);
        newXS("Unicode::Collate::_fetch_simple", XS_Unicode__Collate__fetch_simple, file);
        cv = newXS("Unicode::Collate::_exists_simple", XS_Unicode__Collate__ignorable_simple, file);
        XSANY.any_i32 = 1;
        cv = newXS("Unicode::Collate::_ignorable_simple", XS_Unicode__Collate__ignorable_simple, file);
        XSANY.any_i32 = 0;
        newXS("Unicode::Collate::_getHexArray", XS_Unicode__Collate__getHexArray, file);
        newXS("Unicode::Collate::_isIllegal", XS_Unicode__Collate__isIllegal, file);
        newXS("Unicode::Collate::_decompHangul", XS_Unicode__Collate__decompHangul, file);
        newXS("Unicode::Collate::getHST", XS_Unicode__Collate_getHST, file);
        cv = newXS("Unicode::Collate::_derivCE_14", XS_Unicode__Collate__derivCE_9, file);
        XSANY.any_i32 = 1;
        cv = newXS("Unicode::Collate::_derivCE_18", XS_Unicode__Collate__derivCE_9, file);
        XSANY.any_i32 = 2;
        cv = newXS("Unicode::Collate::_derivCE_20", XS_Unicode__Collate__derivCE_9, file);
        XSANY.any_i32 = 3;
        cv = newXS("Unicode::Collate::_derivCE_22", XS_Unicode__Collate__derivCE_9, file);
        XSANY.any_i32 = 4;
        cv = newXS("Unicode::Collate::_derivCE_24", XS_Unicode__Collate__derivCE_9, file);
        XSANY.any_i32 = 5;
        cv = newXS("Unicode::Collate::_derivCE_9", XS_Unicode__Collate__derivCE_9, file);
        XSANY.any_i32 = 0;
        newXS("Unicode::Collate::_derivCE_8", XS_Unicode__Collate__derivCE_8, file);
        newXS("Unicode::Collate::_uideoCE_8", XS_Unicode__Collate__uideoCE_8, file);
        newXS("Unicode::Collate::_isUIdeo", XS_Unicode__Collate__isUIdeo, file);
        newXS("Unicode::Collate::mk_SortKey", XS_Unicode__Collate_mk_SortKey, file);
        newXS("Unicode::Collate::varCE", XS_Unicode__Collate_varCE, file);
        newXS("Unicode::Collate::visualizeSortKey", XS_Unicode__Collate_visualizeSortKey, file);
        newXS("Unicode::Collate::unpackUfor56", XS_Unicode__Collate_unpackUfor56, file);
#if (PERL_REVISION == 5 && PERL_VERSION >= 9)
  if (PL_unitcheckav)
       call_list(PL_scopestack_ix, PL_unitcheckav);
#endif
    XSRETURN_YES;
}

