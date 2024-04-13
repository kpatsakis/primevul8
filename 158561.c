smtp_get_cache(void)
{
#ifndef DISABLE_DKIM
int n = smtp_inend - smtp_inptr;
if (n > 0)
  dkim_exim_verify_feed(smtp_inptr, n);
#endif
}