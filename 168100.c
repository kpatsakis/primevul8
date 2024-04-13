dirvote_clear_votes(int all_votes)
{
  if (!previous_vote_list)
    previous_vote_list = smartlist_new();
  if (!pending_vote_list)
    pending_vote_list = smartlist_new();

  /* All "previous" votes are now junk. */
  SMARTLIST_FOREACH(previous_vote_list, pending_vote_t *, v, {
      cached_dir_decref(v->vote_body);
      v->vote_body = NULL;
      networkstatus_vote_free(v->vote);
      tor_free(v);
    });
  smartlist_clear(previous_vote_list);

  if (all_votes) {
    /* If we're dumping all the votes, we delete the pending ones. */
    SMARTLIST_FOREACH(pending_vote_list, pending_vote_t *, v, {
        cached_dir_decref(v->vote_body);
        v->vote_body = NULL;
        networkstatus_vote_free(v->vote);
        tor_free(v);
      });
  } else {
    /* Otherwise, we move them into "previous". */
    smartlist_add_all(previous_vote_list, pending_vote_list);
  }
  smartlist_clear(pending_vote_list);

  if (pending_consensus_signature_list) {
    SMARTLIST_FOREACH(pending_consensus_signature_list, char *, cp,
                      tor_free(cp));
    smartlist_clear(pending_consensus_signature_list);
  }
  tor_free(pending_consensus_signatures);
  dirvote_clear_pending_consensuses();
}