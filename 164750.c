network_reentry_is_allowed(void)
{
  /* Default is false, re-entry is not allowed. */
  return !!networkstatus_get_param(NULL, "allow-network-reentry", 0, 0, 1);
}