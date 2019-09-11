/* Copyright (C) 1992, 1995, 1996, 1999 Free Software Foundation, Inc.
This file is part of the GNU C Library.

The GNU C Library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

The GNU C Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with the GNU C Library; if not, write to the Free
Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.  */

#ifndef _IEEE754_H

#define _IEEE754_H 1


#define __BIG_ENDIAN	1
#define __LITTLE_ENDIAN 0
#define __BYTE_ORDER __LITTLE_ENDIAN


union ieee754_float
{
	float f;

	/* This is the IEEE 754 single-precision format.  */
	struct
	{
#if	__BYTE_ORDER == __BIG_ENDIAN
		unsigned int negative : 1;
		unsigned int exponent : 8;
		unsigned int mantissa : 23;
#endif				/* Big endian.  */
#if	__BYTE_ORDER == __LITTLE_ENDIAN
		unsigned int mantissa : 23;
		unsigned int exponent : 8;
		unsigned int negative : 1;
#endif				/* Little endian.  */
	} ieee;

	/* This format makes it easier to see if a NaN is a signalling NaN.  */
	struct
	{
#if	__BYTE_ORDER == __BIG_ENDIAN
		unsigned int negative : 1;
		unsigned int exponent : 8;
		unsigned int quiet_nan : 1;
		unsigned int mantissa : 22;
#endif				/* Big endian.  */
#if	__BYTE_ORDER == __LITTLE_ENDIAN
		unsigned int mantissa : 22;
		unsigned int quiet_nan : 1;
		unsigned int exponent : 8;
		unsigned int negative : 1;
#endif				/* Little endian.  */
	} ieee_nan;
};

#define IEEE754_FLOAT_BIAS	0x7f /* Added to exponent.  */


#endif /* ieee754.h */