/*
 *	babyos/lib/libm/s_scalbn.c
 *
 *  Copyright (C) <2020>  <Ruyi Liu>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */


/*
 *  2020-03-17		from FreeBSD
 */

/* @(#)s_scalbn.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */


/*
 * scalbn (double x, int n)
 * scalbn(x,n) returns x* 2**n  computed by  exponent
 * manipulation rather than by actually performing an
 * exponentiation or a multiplication.
 */

#include <float.h>

#include "math.h"
#include "math_private.h"

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
        twom54  =  5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
        huge   = 1.0e+300,
        tiny   = 1.0e-300;

    double
scalbn (double x, int n)
{
    int32_t k,hx,lx;
    EXTRACT_WORDS(hx,lx,x);
    k = (hx&0x7ff00000)>>20;		/* extract exponent */
    if (k==0) {				/* 0 or subnormal x */
        if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
        x *= two54;
        GET_HIGH_WORD(hx,x);
        k = ((hx&0x7ff00000)>>20) - 54;
        if (n< -50000) return tiny*x; 	/*underflow*/
    }
    if (k==0x7ff) return x+x;		/* NaN or Inf */
    k = k+n;
    if (k >  0x7fe) return huge*copysign(huge,x); /* overflow  */
    if (k > 0) 				/* normal result */
    {SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20)); return x;}
    if (k <= -54) {
        if (n > 50000) 	/* in case integer overflow in n+k */
            return huge*copysign(huge,x);	/*overflow*/
        else return tiny*copysign(tiny,x); 	/*underflow*/
    }
    k += 54;				/* subnormal result */
    SET_HIGH_WORD(x,(hx&0x800fffff)|(k<<20));
    return x*twom54;
}

#if (LDBL_MANT_DIG == 53)
__weak_reference(scalbn, ldexpl);
__weak_reference(scalbn, scalbnl);
#endif
