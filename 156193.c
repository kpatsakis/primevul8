onig_set_retry_limit_in_match(unsigned long size)
{
#ifdef USE_RETRY_LIMIT_IN_MATCH
  RetryLimitInMatch = size;
  return 0;
#else
  return ONIG_NO_SUPPORT_CONFIG;
#endif
}