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


#include <stdio.h>
#include <unistd.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>

#include "hexdump.h"

// Dump a null-character terminated buffer
void utilHexdumpBufs(FILE *out, const void *buff)
{
	size_t	l;
	if (buff==NULL)
	{
		utilHexdumpBuf(out, NULL, 0);
		return;
	}
	l = strlen(buff);
	utilHexdumpBuf(out, buff, l);
}

// Dump a binary buffer
void utilHexdumpBuf(FILE *out, const void *buff, size_t buflen)
{
	int		ch;
	int		c = 0;
	int		i = 0;
	size_t	j;
	unsigned char	ascii[20];  /* to hold printable portion of string */

	if (buff==NULL)
	{
		fprintf(out, "    (NULL)\n");
		return;
	}
	if (buflen==0)
	{
		fprintf(out, "    0000:                                                 **\n");
		return;
	}
	for(j=0; j<buflen; j++)
	{
		ch = ((char*)buff)[j];
		if ((c % 16) == 0)
		{
			if (j)
			{
				ascii[i] = '\0';
				fprintf(out, " *%s*\n", ascii);
			}
			fprintf(out, "    %4.4X:", c);
			i = 0;
		}
		fprintf(out, " %2.2X", ch & 0xff);

		ascii[i] = ch;
		if ((ascii[i] >= 0x80)||(!isprint(ascii[i])))
			ascii[i] = '.';
		++i;
		++c;
	}
	while (c%16)
	{
		fprintf(out, "   ");
		++c;
	}
	ascii[i] = '\0';
	fprintf(out, " *%s*\n", ascii);
}



//*************************************************************************
// Convert a binary byte array into printable hex bytes.
//*************************************************************************
void bin2hex(
    unsigned char *bin,
    unsigned char *hex,
    size_t len
    )
{
	int l;
    for(l=0; l<len; l++)
        sprintf((char*)(&hex[l*2]), "%02x", bin[l]);
}

//*************************************************************************
// Convert printable hex bytes into a binary string.
// It is assumed that the input hex string is valid.
// Returns the number of bytes converted.
//*************************************************************************
size_t hex2bin(
    unsigned char *hex,
    unsigned char *bin
    )
{
    int l;
    for (l=0; hex[l*2]; l++)
    {
        int n;
        n = hex[l*2]-'0';
        if (n>9)
            n -= 'A'-'9'-1;
        if (n>0x0f)
            n -= 'a'-'A';
        bin[l] = n << 4;

        n = hex[l*2+1]-'0';
        if (n>9)
            n -= 'A'-'9'-1;
        if (n>0x0f)
            n -= 'a'-'A';
        bin[l] |= n;
    }
    return(l);
}
