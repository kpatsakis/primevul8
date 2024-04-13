dirvote_add_signatures_to_all_pending_consensuses(
                       const char *detached_signatures_body,
                       const char *source,
                       const char **msg_out)
{
  int r=0, i, n_added = 0, errors = 0;
  ns_detached_signatures_t *sigs;
  tor_assert(detached_signatures_body);
  tor_assert(msg_out);
  tor_assert(pending_consensus_signatures);

  if (!(sigs = networkstatus_parse_detached_signatures(
                               detached_signatures_body, NULL))) {
    *msg_out = "Couldn't parse detached signatures.";
    goto err;
  }

  for (i = 0; i < N_CONSENSUS_FLAVORS; ++i) {
    int res;
    int severity = i == FLAV_NS ? LOG_NOTICE : LOG_INFO;
    pending_consensus_t *pc = &pending_consensuses[i];
    if (!pc->consensus)
      continue;
    res = dirvote_add_signatures_to_pending_consensus(pc, sigs, source,
                                                      severity, msg_out);
    if (res < 0)
      errors++;
    else
      n_added += res;
  }

  if (errors && !n_added) {
    r = -1;
    goto err;
  }

  if (n_added && pending_consensuses[FLAV_NS].consensus) {
    char *new_detached =
      get_detached_signatures_from_pending_consensuses(
                      pending_consensuses, N_CONSENSUS_FLAVORS);
    if (new_detached) {
      tor_free(pending_consensus_signatures);
      pending_consensus_signatures = new_detached;
    }
  }

  r = n_added;
  goto done;
 err:
  if (!*msg_out)
    *msg_out = "Unrecognized error while adding detached signatures.";
 done:
  ns_detached_signatures_free(sigs);
  /* XXXX NM Check how return is used.  We can now have an error *and*
     signatures added. */
  return r;
}