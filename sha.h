/*
------------------------------------------------------------------
  Copyright
  Sun Microsystems, Inc.


  Copyright (C) 1994, 1995, 1996 Sun Microsystems, Inc.  All Rights
  Reserved.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software or derivatives of the Software, and to 
  permit persons to whom the Software or its derivatives is furnished 
  to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT.  IN NO EVENT SHALL SUN MICROSYSTEMS, INC., BE LIABLE
  FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR DERIVATES OF THIS SOFTWARE OR 
  THE USE OR OTHER DEALINGS IN THE SOFTWARE.

  Except as contained in this notice, the name of Sun Microsystems, Inc.
  shall not be used in advertising or otherwise to promote
  the sale, use or other dealings in this Software or its derivatives 
  without prior written authorization from Sun Microsystems, Inc.
*/

#pragma ident "@(#)sha.h	1.2 96/01/29 Sun Microsystems"

#ifndef SHA_H
#define SHA_H

/*
 * NIST Secure Hash Algorithm.
 *
 * Written 2 September 1992, Peter C. Gutmann.
 * This implementation placed in the public domain.
 *
 * Modified 1 June 1993, Colin Plumb.
 * Renamed to SHA and comments updated a bit 1 November 1995 Colin Plumb.
 * These modifications placed in the public domain.
 *
 * Comments to pgut1@cs.aukuni.ac.nz
 */

/* Typedefs for various word sizes */
typedef unsigned long word32;
typedef unsigned char word8;

/* The SHA block size and message digest sizes, in bytes */
#define SHA_BLOCKSIZE	64
#define SHA_DIGESTSIZE	20

/*
 * The structure for storing SHA info.
 * data[] is placed first in case offsets of 0 are faster
 * for some reason; it's the most often accessed field.
 */

struct SHAContext {
	word32 data[ 16 ];		/* SHA data buffer */
	word32 digest[ 5 ];		/* Message digest */
#ifdef HAVE64
	word64 count;
#else
	word32 countHi, countLo;	/* 64-bit byte count */
#endif
};

/* Which standard?  FIPS 180 or FIPS 180.1? */

#define SHA_VERSION 1

/* Whether the machine is little-endian or not */

#if !defined(BIG_ENDIAN) && !defined(LITTLE_ENDIAN)
#define BIG_ENDIAN 1
#endif

#ifdef __cplusplus
extern "C" {
#endif

void shaInit(struct SHAContext *sha);
void shaUpdate(struct SHAContext *sha, word8 const *buffer, unsigned count);
void shaFinal(struct SHAContext *shaInfo, word8 *hash);
void shaTransform(struct SHAContext *sha);

#ifdef __cplusplus
}
#endif

#endif
