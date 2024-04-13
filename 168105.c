dirvote_add_signatures(const char *detached_signatures_body,
                       const char *source,
                       const char **msg)
{
  if (pending_consensuses[FLAV_NS].consensus) {
    log_notice(LD_DIR, "Got a signature from %s. "
                       "Adding it to the pending consensus.", source);
    return dirvote_add_signatures_to_all_pending_consensuses(
                                     detached_signatures_body, source, msg);
  } else {
    log_notice(LD_DIR, "Got a signature from %s. "
                       "Queuing it for the next consensus.", source);
    if (!pending_consensus_signature_list)
      pending_consensus_signature_list = smartlist_new();
    smartlist_add(pending_consensus_signature_list,
                  tor_strdup(detached_signatures_body));
    *msg = "Signature queued";
    return 0;
  }
}