smtp_getc(unsigned lim)
{
if (smtp_inptr >= smtp_inend)
  if (!smtp_refill(lim))
    return EOF;
return *smtp_inptr++;
}