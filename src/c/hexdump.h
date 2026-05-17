/****************************************************************************
*
*    Copyright (C) 2015,2026 John Winans
*
*    This library is free software; you can redistribute it and/or
*    modify it under the terms of the GNU Lesser General Public
*    License as published by the Free Software Foundation; either
*    version 2.1 of the License, or (at your option) any later version.
*
*    This library is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
*    Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public
*    License along with this library; if not, write to the Free Software
*    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
*    USA
*
****************************************************************************/

#ifndef UTIL_HEXDUMP_H
#define UTIL_HEXDUMP_H

#if     defined(__cplusplus)
extern "C" {
#endif

#include <stdio.h>

void utilHexdumpBuf(FILE *out, const void *buff, size_t buflen);
void utilHexdumpBufs(FILE *out, const void *buff);
size_t hex2bin(unsigned char *hex, unsigned char *bin);
void bin2hex(unsigned char *bin, unsigned char *hex, size_t len);

#if     defined(__cplusplus)
}
#endif


#endif
