dirvote_add_signatures_to_pending_consensus(
                       pending_consensus_t *pc,
                       ns_detached_signatures_t *sigs,
                       const char *source,
                       int severity,
                       const char **msg_out)
{
  const char *flavor_name;
  int r = -1;

  /* Only call if we have a pending consensus right now. */
  tor_assert(pc->consensus);
  tor_assert(pc->body);
  tor_assert(pending_consensus_signatures);

  flavor_name = networkstatus_get_flavor_name(pc->consensus->flavor);
  *msg_out = NULL;

  {
    smartlist_t *sig_list = strmap_get(sigs->signatures, flavor_name);
    log_info(LD_DIR, "Have %d signatures for adding to %s consensus.",
             sig_list ? smartlist_len(sig_list) : 0, flavor_name);
  }
  r = networkstatus_add_detached_signatures(pc->consensus, sigs,
                                            source, severity, msg_out);
  log_info(LD_DIR,"Added %d signatures to consensus.", r);

  if (r >= 1) {
    char *new_signatures =
      networkstatus_format_signatures(pc->consensus, 0);
    char *dst, *dst_end;
    size_t new_consensus_len;
    if (!new_signatures) {
      *msg_out = "No signatures to add";
      goto err;
    }
    new_consensus_len =
      strlen(pc->body) + strlen(new_signatures) + 1;
    pc->body = tor_realloc(pc->body, new_consensus_len);
    dst_end = pc->body + new_consensus_len;
    dst = strstr(pc->body, "directory-signature ");
    tor_assert(dst);
    strlcpy(dst, new_signatures, dst_end-dst);

    /* We remove this block once it has failed to crash for a while.  But
     * unless it shows up in profiles, we're probably better leaving it in,
     * just in case we break detached signature processing at some point. */
    {
      networkstatus_t *v = networkstatus_parse_vote_from_string(
                                             pc->body, NULL,
                                             NS_TYPE_CONSENSUS);
      tor_assert(v);
      networkstatus_vote_free(v);
    }
    *msg_out = "Signatures added";
    tor_free(new_signatures);
  } else if (r == 0) {
    *msg_out = "Signatures ignored";
  } else {
    goto err;
  }

  goto done;
 err:
  if (!*msg_out)
    *msg_out = "Unrecognized error while adding detached signatures.";
 done:
  return r;
}