dirvote_fetch_missing_signatures(void)
{
  int need_any = 0;
  int i;
  for (i=0; i < N_CONSENSUS_FLAVORS; ++i) {
    networkstatus_t *consensus = pending_consensuses[i].consensus;
    if (!consensus ||
        networkstatus_check_consensus_signature(consensus, -1) == 1) {
      /* We have no consensus, or we have one that's signed by everybody. */
      continue;
    }
    need_any = 1;
  }
  if (!need_any)
    return;

  directory_get_from_all_authorities(DIR_PURPOSE_FETCH_DETACHED_SIGNATURES,
                                     0, NULL);
}