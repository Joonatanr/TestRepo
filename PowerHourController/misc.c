/*
 * misc.c
 *
 *  Created on: 21.02.2016
 *      Author: Joonatan
 */
#include "misc.h"


unsigned char addchar (char * str, char c)
{
	unsigned char pos = 0u;
	char *ps = str;
	while (ps)
	{
		if (ps[0] == 0)
		{
			break;
		}
		ps++;
		pos++;
	}
	str[pos] = c;
	str[++pos] = 0;
	return pos;
}

unsigned char addstr (char * str1, const char * str2)
{
	unsigned char pos1 = 0u;

	char * ps1 = str1;
	const char * ps2 = str2;

	while (ps1)
	{
		if (ps1[0] == 0)
		{
			break;
		}
		ps1++;
		pos1++;
	}

	while (ps2)
	{
		if (ps2[0] == 0)
		{
			break;
		}
		str1[pos1] = ps2[0];
		ps2++;
		pos1++;
	}
	str1[pos1] = 0;
	return pos1;
}



void long2string (long nr, char *s)
{
  unsigned char c, i, imax, imax2, i2;

  i2 = 0; i = 0;
  if (nr == 0) s[i++] = '0';
  if (nr < 0) {i2 = 1; nr = 0 - nr; }

  while (nr>0)
  {
    c = nr % 10;
    nr = nr / 10;
    s[i++] = '0' + c;
  }
  if (i2) s[i++] = '-';
  s[i] = 0;

  imax = i;
  imax2 = imax/2;
  for (i=0; i<imax2; ++i) {
    i2 = imax-i-1;
    c = s[i];
    s[i] = s[i2];
    s[i2] = c;
  }
}



