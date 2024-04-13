static struct rfc2231_parameter *rfc2231_new_parameter (void)
{
  return safe_calloc (sizeof (struct rfc2231_parameter), 1);
}