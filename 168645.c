static void rfc2231_free_parameter (struct rfc2231_parameter **p)
{
  if (*p)
  {
    FREE (&(*p)->attribute);
    FREE (&(*p)->value);
    FREE (p);		/* __FREE_CHECKED__ */
  }
}