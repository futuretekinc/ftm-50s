/*
 * This file was generated automatically by ExtUtils::ParseXS version 3.24 from the
 * contents of Normalize.xs. Do not edit this file, edit Normalize.xs instead.
 *
 *    ANY CHANGES MADE HERE WILL BE LOST!
 *
 */

#line 1 "Normalize.xs"

#define PERL_NO_GET_CONTEXT /* we want efficiency */

/* private functions which need pTHX_ and aTHX_
    pv_cat_decompHangul
    sv_2pvunicode
    pv_utf8_decompose
    pv_utf8_reorder
    pv_utf8_compose
*/

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

/* These 5 files are prepared by mkheader */
#include "unfcmb.h"
#include "unfcan.h"
#include "unfcpt.h"
#include "unfcmp.h"
#include "unfexc.h"

/* Perl 5.6.1 ? */
#ifndef uvuni_to_utf8
#define uvuni_to_utf8   uv_to_utf8
#endif /* uvuni_to_utf8 */

/* Perl 5.6.1 ? */
#ifndef utf8n_to_uvuni
#define utf8n_to_uvuni   utf8_to_uv
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

/* check if the string buffer is enough before uvuni_to_utf8(). */
/* dstart, d, and dlen should be defined outside before. */
#define Renew_d_if_not_enough_to(need)	STRLEN curlen = d - dstart;	\
		if (dlen < curlen + (need)) {	\
		    dlen += (need);		\
		    Renew(dstart, dlen+1, U8);	\
		    d = dstart + curlen;	\
		}

/* if utf8n_to_uvuni() sets retlen to 0 (if broken?) */
#define ErrRetlenIsZero "panic (Unicode::Normalize %s): zero-length character"

/* utf8_hop() hops back before start. Maybe broken UTF-8 */
#define ErrHopBeforeStart "panic (Unicode::Normalize): hopping before start"

/* At present, char > 0x10ffff are unaffected without complaint, right? */
#define VALID_UTF_MAX    (0x10ffff)
#define OVER_UTF_MAX(uv) (VALID_UTF_MAX < (uv))

/* size of array for combining characters */
/* enough as an initial value? */
#define CC_SEQ_SIZE (10)
#define CC_SEQ_STEP  (5)

/* HANGUL begin */
#define Hangul_SBase  0xAC00
#define Hangul_SFinal 0xD7A3
#define Hangul_SCount  11172

#define Hangul_NCount    588

#define Hangul_LBase  0x1100
#define Hangul_LFinal 0x1112
#define Hangul_LCount     19

#define Hangul_VBase  0x1161
#define Hangul_VFinal 0x1175
#define Hangul_VCount     21

#define Hangul_TBase  0x11A7
#define Hangul_TFinal 0x11C2
#define Hangul_TCount     28

#define Hangul_IsS(u)  ((Hangul_SBase <= (u)) && ((u) <= Hangul_SFinal))
#define Hangul_IsN(u)  (((u) - Hangul_SBase) % Hangul_TCount == 0)
#define Hangul_IsLV(u) (Hangul_IsS(u) && Hangul_IsN(u))
#define Hangul_IsL(u)  ((Hangul_LBase <= (u)) && ((u) <= Hangul_LFinal))
#define Hangul_IsV(u)  ((Hangul_VBase <= (u)) && ((u) <= Hangul_VFinal))
#define Hangul_IsT(u)  ((Hangul_TBase  < (u)) && ((u) <= Hangul_TFinal))
/* HANGUL end */

/* this is used for canonical ordering of combining characters (c.c.). */
typedef struct {
    U8 cc;	/* combining class */
    UV uv;	/* codepoint */
    STRLEN pos; /* position */
} UNF_cc;

static int compare_cc(const void *a, const void *b)
{
    int ret_cc;
    ret_cc = ((UNF_cc*) a)->cc - ((UNF_cc*) b)->cc;
    if (ret_cc)
	return ret_cc;

    return ( ((UNF_cc*) a)->pos > ((UNF_cc*) b)->pos )
	 - ( ((UNF_cc*) a)->pos < ((UNF_cc*) b)->pos );
}

static U8* dec_canonical(UV uv)
{
    U8 ***plane, **row;
    if (OVER_UTF_MAX(uv))
	return NULL;
    plane = (U8***)UNF_canon[uv >> 16];
    if (! plane)
	return NULL;
    row = plane[(uv >> 8) & 0xff];
    return row ? row[uv & 0xff] : NULL;
}

static U8* dec_compat(UV uv)
{
    U8 ***plane, **row;
    if (OVER_UTF_MAX(uv))
	return NULL;
    plane = (U8***)UNF_compat[uv >> 16];
    if (! plane)
	return NULL;
    row = plane[(uv >> 8) & 0xff];
    return row ? row[uv & 0xff] : NULL;
}

static UV composite_uv(UV uv, UV uv2)
{
    UNF_complist ***plane, **row, *cell, *i;

    if (!uv2 || OVER_UTF_MAX(uv) || OVER_UTF_MAX(uv2))
	return 0;

    if (Hangul_IsL(uv) && Hangul_IsV(uv2)) {
	UV lindex = uv  - Hangul_LBase;
	UV vindex = uv2 - Hangul_VBase;
	return(Hangul_SBase + (lindex * Hangul_VCount + vindex) *
	       Hangul_TCount);
    }
    if (Hangul_IsLV(uv) && Hangul_IsT(uv2)) {
	UV tindex = uv2 - Hangul_TBase;
	return(uv + tindex);
    }
    plane = UNF_compos[uv >> 16];
    if (! plane)
	return 0;
    row = plane[(uv >> 8) & 0xff];
    if (! row)
	return 0;
    cell = row[uv & 0xff];
    if (! cell)
	return 0;
    for (i = cell; i->nextchar; i++) {
	if (uv2 == i->nextchar)
	    return i->composite;
    }
    return 0;
}

static U8 getCombinClass(UV uv)
{
    U8 **plane, *row;
    if (OVER_UTF_MAX(uv))
	return 0;
    plane = (U8**)UNF_combin[uv >> 16];
    if (! plane)
	return 0;
    row = plane[(uv >> 8) & 0xff];
    return row ? row[uv & 0xff] : 0;
}

static U8* pv_cat_decompHangul(pTHX_ U8* d, UV uv)
{
    UV sindex =  uv - Hangul_SBase;
    UV lindex =  sindex / Hangul_NCount;
    UV vindex = (sindex % Hangul_NCount) / Hangul_TCount;
    UV tindex =  sindex % Hangul_TCount;

    if (! Hangul_IsS(uv))
	return d;

    d = uvuni_to_utf8(d, (lindex + Hangul_LBase));
    d = uvuni_to_utf8(d, (vindex + Hangul_VBase));
    if (tindex)
	d = uvuni_to_utf8(d, (tindex + Hangul_TBase));
    return d;
}

static char* sv_2pvunicode(pTHX_ SV *sv, STRLEN *lp)
{
    char *s;
    STRLEN len;
    s = SvPV(sv,len);
    if (!SvUTF8(sv)) {
	SV* tmpsv = sv_2mortal(newSVpvn(s, len));
	if (!SvPOK(tmpsv))
	    s = SvPV_force(tmpsv,len);
	sv_utf8_upgrade(tmpsv);
	s = SvPV(tmpsv,len);
    }
    if (lp)
	*lp = len;
    return s;
}

static
U8* pv_utf8_decompose(pTHX_ U8* s, STRLEN slen, U8** dp, STRLEN dlen, bool iscompat)
{
    U8* p = s;
    U8* e = s + slen;
    U8* dstart = *dp;
    U8* d = dstart;

    while (p < e) {
	STRLEN retlen;
	UV uv = utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF);
	if (!retlen)
	    croak(ErrRetlenIsZero, "decompose");
	p += retlen;

	if (Hangul_IsS(uv)) {
	    Renew_d_if_not_enough_to(UTF8_MAXLEN * 3)
	    d = pv_cat_decompHangul(aTHX_ d, uv);
	}
	else {
	    U8* r = iscompat ? dec_compat(uv) : dec_canonical(uv);

	    if (r) {
		STRLEN len = (STRLEN)strlen((char *)r);
		Renew_d_if_not_enough_to(len)
		while (len--)
		    *d++ = *r++;
	    }
	    else {
		Renew_d_if_not_enough_to(UTF8_MAXLEN)
		d = uvuni_to_utf8(d, uv);
	    }
	}
    }
    *dp = dstart;
    return d;
}

static
U8* pv_utf8_reorder(pTHX_ U8* s, STRLEN slen, U8** dp, STRLEN dlen)
{
    U8* p = s;
    U8* e = s + slen;
    U8* dstart = *dp;
    U8* d = dstart;

    UNF_cc  seq_ary[CC_SEQ_SIZE];
    UNF_cc* seq_ptr = seq_ary; /* use array at the beginning */
    UNF_cc* seq_ext = NULL; /* extend if need */
    STRLEN seq_max = CC_SEQ_SIZE;
    STRLEN cc_pos = 0;

    while (p < e) {
	U8 curCC;
	STRLEN retlen;
	UV uv = utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF);
	if (!retlen)
	    croak(ErrRetlenIsZero, "reorder");
	p += retlen;

	curCC = getCombinClass(uv);

	if (curCC != 0) {
	    if (seq_max < cc_pos + 1) { /* extend if need */
		seq_max = cc_pos + CC_SEQ_STEP; /* new size */
		if (CC_SEQ_SIZE == cc_pos) { /* seq_ary full */
		    STRLEN i;
		    New(0, seq_ext, seq_max, UNF_cc);
		    for (i = 0; i < cc_pos; i++)
			seq_ext[i] = seq_ary[i];
		}
		else {
		    Renew(seq_ext, seq_max, UNF_cc);
		}
		seq_ptr = seq_ext; /* use seq_ext from now */
	    }

	    seq_ptr[cc_pos].cc  = curCC;
	    seq_ptr[cc_pos].uv  = uv;
	    seq_ptr[cc_pos].pos = cc_pos;
	    ++cc_pos;

	    if (p < e)
		continue;
	}

	/* output */
	if (cc_pos) {
	    STRLEN i;

	    if (cc_pos > 1) /* reordered if there are two c.c.'s */
		qsort((void*)seq_ptr, cc_pos, sizeof(UNF_cc), compare_cc);

	    for (i = 0; i < cc_pos; i++) {
		Renew_d_if_not_enough_to(UTF8_MAXLEN)
		d = uvuni_to_utf8(d, seq_ptr[i].uv);
	    }
	    cc_pos = 0;
	}

	if (curCC == 0) {
	    Renew_d_if_not_enough_to(UTF8_MAXLEN)
	    d = uvuni_to_utf8(d, uv);
	}
    }
    if (seq_ext)
	Safefree(seq_ext);
    *dp = dstart;
    return d;
}

static
U8* pv_utf8_compose(pTHX_ U8* s, STRLEN slen, U8** dp, STRLEN dlen, bool iscontig)
{
    U8* p = s;
    U8* e = s + slen;
    U8* dstart = *dp;
    U8* d = dstart;

    UV uvS = 0; /* code point of the starter */
    bool valid_uvS = FALSE; /* if FALSE, uvS isn't initialized yet */
    U8 preCC = 0;

    UV  seq_ary[CC_SEQ_SIZE];
    UV* seq_ptr = seq_ary; /* use array at the beginning */
    UV* seq_ext = NULL; /* extend if need */
    STRLEN seq_max = CC_SEQ_SIZE;
    STRLEN cc_pos = 0;

    while (p < e) {
	U8 curCC;
	STRLEN retlen;
	UV uv = utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF);
	if (!retlen)
	    croak(ErrRetlenIsZero, "compose");
	p += retlen;

	curCC = getCombinClass(uv);

	if (!valid_uvS) {
	    if (curCC == 0) {
		uvS = uv; /* the first Starter is found */
		valid_uvS = TRUE;
		if (p < e)
		    continue;
	    }
	    else {
		Renew_d_if_not_enough_to(UTF8_MAXLEN)
		d = uvuni_to_utf8(d, uv);
		continue;
	    }
	}
	else {
	    bool composed;

	    /* blocked */
	    if ((iscontig && cc_pos) || /* discontiguous combination */
		 (curCC != 0 && preCC == curCC) || /* blocked by same CC */
		 (preCC > curCC)) /* blocked by higher CC: revised D2 */
		composed = FALSE;

	    /* not blocked:
		 iscontig && cc_pos == 0      -- contiguous combination
		 curCC == 0 && preCC == 0     -- starter + starter
		 curCC != 0 && preCC < curCC  -- lower CC */
	    else {
		/* try composition */
		UV uvComp = composite_uv(uvS, uv);

		if (uvComp && !isExclusion(uvComp))  {
		    uvS = uvComp;
		    composed = TRUE;

		    /* preCC should not be changed to curCC */
		    /* e.g. 1E14 = 0045 0304 0300 where CC(0304) == CC(0300) */
		    if (p < e)
			continue;
		}
		else
		    composed = FALSE;
	    }

	    if (!composed) {
		preCC = curCC;
		if (curCC != 0 || !(p < e)) {
		    if (seq_max < cc_pos + 1) { /* extend if need */
			seq_max = cc_pos + CC_SEQ_STEP; /* new size */
			if (CC_SEQ_SIZE == cc_pos) { /* seq_ary full */
			    New(0, seq_ext, seq_max, UV);
			    Copy(seq_ary, seq_ext, cc_pos, UV);
			}
			else {
			    Renew(seq_ext, seq_max, UV);
			}
			seq_ptr = seq_ext; /* use seq_ext from now */
		    }
		    seq_ptr[cc_pos] = uv;
		    ++cc_pos;
		}
		if (curCC != 0 && p < e)
		    continue;
	    }
	}

	/* output */
	{
	    Renew_d_if_not_enough_to(UTF8_MAXLEN)
	    d = uvuni_to_utf8(d, uvS); /* starter (composed or not) */
	}

	if (cc_pos) {
	    STRLEN i;

	    for (i = 0; i < cc_pos; i++) {
		Renew_d_if_not_enough_to(UTF8_MAXLEN)
		d = uvuni_to_utf8(d, seq_ptr[i]);
	    }
	    cc_pos = 0;
	}

	uvS = uv;
    }
    if (seq_ext)
	Safefree(seq_ext);
    *dp = dstart;
    return d;
}

#line 462 "Normalize.c"
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

#line 604 "Normalize.c"

XS_EUPXS(XS_Unicode__Normalize_decompose); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_decompose)
{
    dVAR; dXSARGS;
    if (items < 1 || items > 2)
       croak_xs_usage(cv,  "src, compat = &PL_sv_no");
    {
	SV *	src = ST(0)
;
	SV *	compat;
#line 460 "Normalize.xs"
    SV* dst;
    U8 *s, *d, *dend;
    STRLEN slen, dlen;
#line 620 "Normalize.c"
	SV *	RETVAL;

	if (items < 2)
	    compat = &PL_sv_no;
	else {
	    compat = ST(1)
;
	}
#line 464 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&slen);
    dst = newSVpvn("", 0);
    dlen = slen;
    New(0, d, dlen+1, U8);
    dend = pv_utf8_decompose(aTHX_ s, slen, &d, dlen, (bool)SvTRUE(compat));
    sv_setpvn(dst, (char *)d, dend - d);
    SvUTF8_on(dst);
    Safefree(d);
    RETVAL = dst;
#line 639 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_reorder); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_reorder)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 482 "Normalize.xs"
    SV* dst;
    U8 *s, *d, *dend;
    STRLEN slen, dlen;
#line 660 "Normalize.c"
	SV *	RETVAL;
#line 486 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&slen);
    dst = newSVpvn("", 0);
    dlen = slen;
    New(0, d, dlen+1, U8);
    dend = pv_utf8_reorder(aTHX_ s, slen, &d, dlen);
    sv_setpvn(dst, (char *)d, dend - d);
    SvUTF8_on(dst);
    Safefree(d);
    RETVAL = dst;
#line 672 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_compose); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_compose)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 506 "Normalize.xs"
    SV* dst;
    U8 *s, *d, *dend;
    STRLEN slen, dlen;
#line 694 "Normalize.c"
	SV *	RETVAL;
#line 510 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&slen);
    dst = newSVpvn("", 0);
    dlen = slen;
    New(0, d, dlen+1, U8);
    dend = pv_utf8_compose(aTHX_ s, slen, &d, dlen, (bool)ix);
    sv_setpvn(dst, (char *)d, dend - d);
    SvUTF8_on(dst);
    Safefree(d);
    RETVAL = dst;
#line 706 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_NFD); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_NFD)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 530 "Normalize.xs"
    SV *dst;
    U8 *s, *t, *tend, *d, *dend;
    STRLEN slen, tlen, dlen;
#line 728 "Normalize.c"
	SV *	RETVAL;
#line 534 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&slen);

    /* decompose */
    tlen = slen;
    New(0, t, tlen+1, U8);
    tend = pv_utf8_decompose(aTHX_ s, slen, &t, tlen, (bool)(ix==1));
    *tend = '\0';
    tlen = tend - t; /* no longer know real size of t */

    /* reorder */
    dlen = tlen;
    New(0, d, dlen+1, U8);
    dend = pv_utf8_reorder(aTHX_ t, tlen, &d, dlen);
    *dend = '\0';
    dlen = dend - d; /* no longer know real size of d */

    /* return */
    dst = newSVpvn("", 0);
    sv_setpvn(dst, (char *)d, dlen);
    SvUTF8_on(dst);

    Safefree(t);
    Safefree(d);
    RETVAL = dst;
#line 755 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_NFC); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_NFC)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 570 "Normalize.xs"
    SV *dst;
    U8 *s, *t, *tend, *u, *uend, *d, *dend;
    STRLEN slen, tlen, ulen, dlen;
#line 777 "Normalize.c"
	SV *	RETVAL;
#line 574 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&slen);

    /* decompose */
    tlen = slen;
    New(0, t, tlen+1, U8);
    tend = pv_utf8_decompose(aTHX_ s, slen, &t, tlen, (bool)(ix==1));
    *tend = '\0';
    tlen = tend - t; /* no longer know real size of t */

    /* reorder */
    ulen = tlen;
    New(0, u, ulen+1, U8);
    uend = pv_utf8_reorder(aTHX_ t, tlen, &u, ulen);
    *uend = '\0';
    ulen = uend - u; /* no longer know real size of u */

    /* compose */
    dlen = ulen;
    New(0, d, dlen+1, U8);
    dend = pv_utf8_compose(aTHX_ u, ulen, &d, dlen, (bool)(ix==2));
    *dend = '\0';
    dlen = dend - d; /* no longer know real size of d */

    /* return */
    dst = newSVpvn("", 0);
    sv_setpvn(dst, (char *)d, dlen);
    SvUTF8_on(dst);

    Safefree(t);
    Safefree(u);
    Safefree(d);
    RETVAL = dst;
#line 812 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_checkNFD); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_checkNFD)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 617 "Normalize.xs"
    STRLEN srclen, retlen;
    U8 *s, *e, *p, curCC, preCC;
    bool result = TRUE;
#line 834 "Normalize.c"
	SV *	RETVAL;
#line 621 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&srclen);
    e = s + srclen;

    preCC = 0;
    for (p = s; p < e; p += retlen) {
	UV uv = utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF);
	if (!retlen)
	    croak(ErrRetlenIsZero, "checkNFD or -NFKD");

	curCC = getCombinClass(uv);
	if (preCC > curCC && curCC != 0) { /* canonical ordering violated */
	    result = FALSE;
	    break;
	}
	if (Hangul_IsS(uv) || (ix ? dec_compat(uv) : dec_canonical(uv))) {
	    result = FALSE;
	    break;
	}
	preCC = curCC;
    }
    RETVAL = boolSV(result);
#line 858 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_checkNFC); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_checkNFC)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 653 "Normalize.xs"
    STRLEN srclen, retlen;
    U8 *s, *e, *p, curCC, preCC;
    bool result = TRUE;
    bool isMAYBE = FALSE;
#line 881 "Normalize.c"
	SV *	RETVAL;
#line 658 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&srclen);
    e = s + srclen;

    preCC = 0;
    for (p = s; p < e; p += retlen) {
	UV uv = utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF);
	if (!retlen)
	    croak(ErrRetlenIsZero, "checkNFC or -NFKC");

	curCC = getCombinClass(uv);
	if (preCC > curCC && curCC != 0) { /* canonical ordering violated */
	    result = FALSE;
	    break;
	}

	/* get NFC/NFKC property */
	if (Hangul_IsS(uv)) /* Hangul syllables are canonical composites */
	    ; /* YES */
	else if (isExclusion(uv) || isSingleton(uv) || isNonStDecomp(uv)) {
	    result = FALSE;
	    break;
	}
	else if (isComp2nd(uv))
	    isMAYBE = TRUE;
	else if (ix) {
	    char *canon, *compat;
	  /* NFKC_NO when having compatibility mapping. */
	    canon  = (char *) dec_canonical(uv);
	    compat = (char *) dec_compat(uv);
	    if (compat && !(canon && strEQ(canon, compat))) {
		result = FALSE;
		break;
	    }
	} /* end of get NFC/NFKC property */

	preCC = curCC;
    }
    if (isMAYBE && result) /* NO precedes MAYBE */
	XSRETURN_UNDEF;
    RETVAL = boolSV(result);
#line 924 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_checkFCD); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_checkFCD)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    {
	SV *	src = ST(0)
;
#line 709 "Normalize.xs"
    STRLEN srclen, retlen;
    U8 *s, *e, *p, curCC, preCC;
    bool result = TRUE;
    bool isMAYBE = FALSE;
#line 947 "Normalize.c"
	SV *	RETVAL;
#line 714 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&srclen);
    e = s + srclen;
    preCC = 0;
    for (p = s; p < e; p += retlen) {
	U8 *sCan;
	UV uvLead;
	STRLEN canlen = 0;
	UV uv = utf8n_to_uvuni(p, e - p, &retlen, AllowAnyUTF);
	if (!retlen)
	    croak(ErrRetlenIsZero, "checkFCD or -FCC");

	sCan = (U8*) dec_canonical(uv);

	if (sCan) {
	    STRLEN canret;
	    canlen = (STRLEN)strlen((char *) sCan);
	    uvLead = utf8n_to_uvuni(sCan, canlen, &canret, AllowAnyUTF);
	    if (!canret)
		croak(ErrRetlenIsZero, "checkFCD or -FCC");
	}
	else {
	    uvLead = uv;
	}

	curCC = getCombinClass(uvLead);

	if (curCC != 0 && curCC < preCC) { /* canonical ordering violated */
	    result = FALSE;
	    break;
	}

	if (ix) {
	    if (isExclusion(uv) || isSingleton(uv) || isNonStDecomp(uv)) {
		result = FALSE;
		break;
	    }
	    else if (isComp2nd(uv))
		isMAYBE = TRUE;
	}

	if (sCan) {
	    STRLEN canret;
	    UV uvTrail;
	    U8* eCan = sCan + canlen;
	    U8* pCan = utf8_hop(eCan, -1);
	    if (pCan < sCan)
		croak(ErrHopBeforeStart);
	    uvTrail = utf8n_to_uvuni(pCan, eCan - pCan, &canret, AllowAnyUTF);
	    if (!canret)
		croak(ErrRetlenIsZero, "checkFCD or -FCC");
	    preCC = getCombinClass(uvTrail);
	}
	else {
	    preCC = curCC;
	}
    }
    if (isMAYBE && result) /* NO precedes MAYBE */
	XSRETURN_UNDEF;
    RETVAL = boolSV(result);
#line 1009 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_getCombinClass); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_getCombinClass)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	U8	RETVAL;
	dXSTARG;

	RETVAL = getCombinClass(uv);
	XSprePUSH; PUSHu((UV)RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_isExclusion); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_isExclusion)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	bool	RETVAL;

	RETVAL = isExclusion(uv);
	ST(0) = boolSV(RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_isSingleton); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_isSingleton)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	bool	RETVAL;

	RETVAL = isSingleton(uv);
	ST(0) = boolSV(RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_isNonStDecomp); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_isNonStDecomp)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	bool	RETVAL;

	RETVAL = isNonStDecomp(uv);
	ST(0) = boolSV(RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_isComp2nd); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_isComp2nd)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	bool	RETVAL;

	RETVAL = isComp2nd(uv);
	ST(0) = boolSV(RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_isNFD_NO); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_isNFD_NO)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
#line 814 "Normalize.xs"
    bool result = FALSE;
#line 1121 "Normalize.c"
	SV *	RETVAL;
#line 816 "Normalize.xs"
    if (Hangul_IsS(uv) || (ix ? dec_compat(uv) : dec_canonical(uv)))
	result = TRUE; /* NFD_NO or NFKD_NO */
    RETVAL = boolSV(result);
#line 1127 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_isComp_Ex); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_isComp_Ex)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
#line 831 "Normalize.xs"
    bool result = FALSE;
#line 1147 "Normalize.c"
	SV *	RETVAL;
#line 833 "Normalize.xs"
    if (isExclusion(uv) || isSingleton(uv) || isNonStDecomp(uv))
	result = TRUE; /* NFC_NO or NFKC_NO */
    else if (ix) {
	char *canon, *compat;
	canon  = (char *) dec_canonical(uv);
	compat = (char *) dec_compat(uv);
	if (compat && (!canon || strNE(canon, compat)))
	    result = TRUE; /* NFC_NO or NFKC_NO */
    }
    RETVAL = boolSV(result);
#line 1160 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_getComposite); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_getComposite)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "uv, uv2");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	UV	uv2 = (UV)SvUV(ST(1))
;
#line 852 "Normalize.xs"
    UV composite;
#line 1181 "Normalize.c"
	SV *	RETVAL;
#line 854 "Normalize.xs"
    composite = composite_uv(uv, uv2);
    RETVAL = composite ? newSVuv(composite) : &PL_sv_undef;
#line 1186 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_getCanon); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_getCanon)
{
    dVAR; dXSARGS;
    dXSI32;
    if (items != 1)
       croak_xs_usage(cv,  "uv");
    {
	UV	uv = (UV)SvUV(ST(0))
;
	SV *	RETVAL;
#line 868 "Normalize.xs"
    if (Hangul_IsS(uv)) {
	U8 tmp[3 * UTF8_MAXLEN + 1];
	U8 *t = tmp;
	U8 *e = pv_cat_decompHangul(aTHX_ t, uv);
	RETVAL = newSVpvn((char *)t, e - t);
    } else {
	U8* rstr = ix ? dec_compat(uv) : dec_canonical(uv);
	if (!rstr)
	    XSRETURN_UNDEF;
	RETVAL = newSVpvn((char *)rstr, strlen((char *)rstr));
    }
    SvUTF8_on(RETVAL);
#line 1218 "Normalize.c"
	ST(0) = RETVAL;
	sv_2mortal(ST(0));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Unicode__Normalize_splitOnLastStarter); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Unicode__Normalize_splitOnLastStarter)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "src");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	SV *	src = ST(0)
;
#line 888 "Normalize.xs"
    SV *svp;
    STRLEN srclen;
    U8 *s, *e, *p;
#line 1241 "Normalize.c"
#line 892 "Normalize.xs"
    s = (U8*)sv_2pvunicode(aTHX_ src,&srclen);
    e = s + srclen;
    p = e;
    while (s < p) {
	UV uv;
	p = utf8_hop(p, -1);
	if (p < s)
	    croak(ErrHopBeforeStart);
	uv = utf8n_to_uvuni(p, e - p, NULL, AllowAnyUTF);
	if (getCombinClass(uv) == 0) /* Last Starter found */
	    break;
    }

    svp = sv_2mortal(newSVpvn((char*)s, p - s));
    SvUTF8_on(svp);
    XPUSHs(svp);

    svp = sv_2mortal(newSVpvn((char*)p, e - p));
    SvUTF8_on(svp);
    XPUSHs(svp);
#line 1263 "Normalize.c"
	PUTBACK;
	return;
    }
}

#ifdef __cplusplus
extern "C"
#endif
XS_EXTERNAL(boot_Unicode__Normalize); /* prototype to pass -Wmissing-prototypes */
XS_EXTERNAL(boot_Unicode__Normalize)
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

        (void)newXSproto_portable("Unicode::Normalize::decompose", XS_Unicode__Normalize_decompose, file, "$;$");
        (void)newXSproto_portable("Unicode::Normalize::reorder", XS_Unicode__Normalize_reorder, file, "$");
        cv = newXSproto_portable("Unicode::Normalize::compose", XS_Unicode__Normalize_compose, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::composeContiguous", XS_Unicode__Normalize_compose, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::NFD", XS_Unicode__Normalize_NFD, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::NFKD", XS_Unicode__Normalize_NFD, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::FCC", XS_Unicode__Normalize_NFC, file, "$");
        XSANY.any_i32 = 2;
        cv = newXSproto_portable("Unicode::Normalize::NFC", XS_Unicode__Normalize_NFC, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::NFKC", XS_Unicode__Normalize_NFC, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::checkNFD", XS_Unicode__Normalize_checkNFD, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::checkNFKD", XS_Unicode__Normalize_checkNFD, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::checkNFC", XS_Unicode__Normalize_checkNFC, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::checkNFKC", XS_Unicode__Normalize_checkNFC, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::checkFCC", XS_Unicode__Normalize_checkFCD, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::checkFCD", XS_Unicode__Normalize_checkFCD, file, "$");
        XSANY.any_i32 = 0;
        (void)newXSproto_portable("Unicode::Normalize::getCombinClass", XS_Unicode__Normalize_getCombinClass, file, "$");
        (void)newXSproto_portable("Unicode::Normalize::isExclusion", XS_Unicode__Normalize_isExclusion, file, "$");
        (void)newXSproto_portable("Unicode::Normalize::isSingleton", XS_Unicode__Normalize_isSingleton, file, "$");
        (void)newXSproto_portable("Unicode::Normalize::isNonStDecomp", XS_Unicode__Normalize_isNonStDecomp, file, "$");
        cv = newXSproto_portable("Unicode::Normalize::isComp2nd", XS_Unicode__Normalize_isComp2nd, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::isNFC_MAYBE", XS_Unicode__Normalize_isComp2nd, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::isNFKC_MAYBE", XS_Unicode__Normalize_isComp2nd, file, "$");
        XSANY.any_i32 = 2;
        cv = newXSproto_portable("Unicode::Normalize::isNFD_NO", XS_Unicode__Normalize_isNFD_NO, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::isNFKD_NO", XS_Unicode__Normalize_isNFD_NO, file, "$");
        XSANY.any_i32 = 1;
        cv = newXSproto_portable("Unicode::Normalize::isComp_Ex", XS_Unicode__Normalize_isComp_Ex, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::isNFC_NO", XS_Unicode__Normalize_isComp_Ex, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::isNFKC_NO", XS_Unicode__Normalize_isComp_Ex, file, "$");
        XSANY.any_i32 = 1;
        (void)newXSproto_portable("Unicode::Normalize::getComposite", XS_Unicode__Normalize_getComposite, file, "$$");
        cv = newXSproto_portable("Unicode::Normalize::getCanon", XS_Unicode__Normalize_getCanon, file, "$");
        XSANY.any_i32 = 0;
        cv = newXSproto_portable("Unicode::Normalize::getCompat", XS_Unicode__Normalize_getCanon, file, "$");
        XSANY.any_i32 = 1;
        newXS("Unicode::Normalize::splitOnLastStarter", XS_Unicode__Normalize_splitOnLastStarter, file);
#if (PERL_REVISION == 5 && PERL_VERSION >= 9)
  if (PL_unitcheckav)
       call_list(PL_scopestack_ix, PL_unitcheckav);
#endif
    XSRETURN_YES;
}

