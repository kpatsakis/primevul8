b64encode(uschar *clear, int len)
{
uschar *code = store_get(4*((len+2)/3) + 1);
uschar *p = code;

while (len-- >0)
  {
  int x, y;

  x = *clear++;
  *p++ = enc64table[(x >> 2) & 63];

  if (len-- <= 0)
    {
    *p++ = enc64table[(x << 4) & 63];
    *p++ = '=';
    *p++ = '=';
    break;
    }

  y = *clear++;
  *p++ = enc64table[((x << 4) | ((y >> 4) & 15)) & 63];

  if (len-- <= 0)
    {
    *p++ = enc64table[(y << 2) & 63];
    *p++ = '=';
    break;
    }

  x = *clear++;
  *p++ = enc64table[((y << 2) | ((x >> 6) & 3)) & 63];

  *p++ = enc64table[x & 63];
  }

*p = 0;

return code;
}