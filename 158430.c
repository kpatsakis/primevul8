smtp_getbuf(unsigned * len)
{
unsigned size;
uschar * buf;

if (smtp_inptr >= smtp_inend)
  if (!smtp_refill(*len))
    { *len = 0; return NULL; }

if ((size = smtp_inend - smtp_inptr) > *len) size = *len;
buf = smtp_inptr;
smtp_inptr += size;
*len = size;
return buf;
}