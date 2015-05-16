/****************************************************************************
*  Copyright (c) 2002-2003 by Michael Fischer. All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*  
*  1. Redistributions of source code must retain the above copyright 
*     notice, this list of conditions and the following disclaimer.
*  2. Redistributions in binary form must reproduce the above copyright
*     notice, this list of conditions and the following disclaimer in the 
*     documentation and/or other materials provided with the distribution.
*  3. Neither the name of the author nor the names of its contributors may 
*     be used to endorse or promote products derived from this software 
*     without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL 
*  THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
*  OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
*  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
*  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
*  SUCH DAMAGE.
*
****************************************************************************
*  History:
*
*  14.12.02  mifi   First Version 
*  18.01.03  mifi   Change Licence from GPL to BSD.
*  07.02.04  mifi   Add some macros.
****************************************************************************/
#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

/*
 * some types to use Windows like source
*/
typedef char CHAR;              /* 8-bit signed data    */
typedef unsigned char BYTE;     /* 8-bit unsigned data  */
typedef unsigned short WORD;    /* 16-bit unsigned data */
typedef long LONG;              /* 32-bit signed data   */
typedef unsigned long ULONG;    /* 32-bit unsigned data */
typedef unsigned long DWORD;    /* 32-bit unsigned data */

/*
 * some define for the BSD files
*/
typedef unsigned char u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned long u_int32_t;

#define FALSE           0
#define TRUE            1

/**************************************************************************
* Some WORD, BYTE macro's
***************************************************************************/
#define HIBYTE(_a) ((((WORD)(_a)) >> 8) & 0x00FF)
#define LOBYTE(_a) (((WORD)(_a)) & 0x00FF)

#define HIWORD(_a) ((((DWORD)(_a)) >> 16) & 0xFFFF)
#define LOWORD(_a) (((DWORD)(_a)) & 0xFFFF)

#define SWAP(_a)	 ((LOBYTE((_a)) << 8) | HIBYTE((_a)))

#endif /* !__TYPEDEFS_H_ */
