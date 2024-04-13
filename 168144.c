dirvote_clear_pending_consensuses(void)
{
  int i;
  for (i = 0; i < N_CONSENSUS_FLAVORS; ++i) {
    pending_consensus_t *pc = &pending_consensuses[i];
    tor_free(pc->body);

    networkstatus_vote_free(pc->consensus);
    pc->consensus = NULL;
  }
}