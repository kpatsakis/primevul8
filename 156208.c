onig_get_retry_limit_in_match(void)
{
#ifdef USE_RETRY_LIMIT_IN_MATCH
  return RetryLimitInMatch;
#else
  /* return ONIG_NO_SUPPORT_CONFIG; */
  return 0;
#endif
}