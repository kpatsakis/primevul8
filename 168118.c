get_detached_signatures_from_pending_consensuses(pending_consensus_t *pending,
                                                 int n_flavors)
{
  int flav;
  char *signatures;
  smartlist_t *c = smartlist_new();
  for (flav = 0; flav < n_flavors; ++flav) {
    if (pending[flav].consensus)
      smartlist_add(c, pending[flav].consensus);
  }
  signatures = networkstatus_get_detached_signatures(c);
  smartlist_free(c);
  return signatures;
}