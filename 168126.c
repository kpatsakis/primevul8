dirvote_add_vote(const char *vote_body, const char **msg_out, int *status_out)
{
  networkstatus_t *vote;
  networkstatus_voter_info_t *vi;
  dir_server_t *ds;
  pending_vote_t *pending_vote = NULL;
  const char *end_of_vote = NULL;
  int any_failed = 0;
  tor_assert(vote_body);
  tor_assert(msg_out);
  tor_assert(status_out);

  if (!pending_vote_list)
    pending_vote_list = smartlist_new();
  *status_out = 0;
  *msg_out = NULL;

 again:
  vote = networkstatus_parse_vote_from_string(vote_body, &end_of_vote,
                                              NS_TYPE_VOTE);
  if (!end_of_vote)
    end_of_vote = vote_body + strlen(vote_body);
  if (!vote) {
    log_warn(LD_DIR, "Couldn't parse vote: length was %d",
             (int)strlen(vote_body));
    *msg_out = "Unable to parse vote";
    goto err;
  }
  tor_assert(smartlist_len(vote->voters) == 1);
  vi = get_voter(vote);
  {
    int any_sig_good = 0;
    SMARTLIST_FOREACH(vi->sigs, document_signature_t *, sig,
                      if (sig->good_signature)
                        any_sig_good = 1);
    tor_assert(any_sig_good);
  }
  ds = trusteddirserver_get_by_v3_auth_digest(vi->identity_digest);
  if (!ds) {
    char *keys = list_v3_auth_ids();
    log_warn(LD_DIR, "Got a vote from an authority (nickname %s, address %s) "
             "with authority key ID %s. "
             "This key ID is not recognized.  Known v3 key IDs are: %s",
             vi->nickname, vi->address,
             hex_str(vi->identity_digest, DIGEST_LEN), keys);
    tor_free(keys);
    *msg_out = "Vote not from a recognized v3 authority";
    goto err;
  }
  tor_assert(vote->cert);
  if (!authority_cert_get_by_digests(vote->cert->cache_info.identity_digest,
                                     vote->cert->signing_key_digest)) {
    /* Hey, it's a new cert! */
    trusted_dirs_load_certs_from_string(
                               vote->cert->cache_info.signed_descriptor_body,
                               TRUSTED_DIRS_CERTS_SRC_FROM_VOTE, 1 /*flush*/,
                               NULL);
    if (!authority_cert_get_by_digests(vote->cert->cache_info.identity_digest,
                                       vote->cert->signing_key_digest)) {
      log_warn(LD_BUG, "We added a cert, but still couldn't find it.");
    }
  }

  /* Is it for the right period? */
  if (vote->valid_after != voting_schedule.interval_starts) {
    char tbuf1[ISO_TIME_LEN+1], tbuf2[ISO_TIME_LEN+1];
    format_iso_time(tbuf1, vote->valid_after);
    format_iso_time(tbuf2, voting_schedule.interval_starts);
    log_warn(LD_DIR, "Rejecting vote from %s with valid-after time of %s; "
             "we were expecting %s", vi->address, tbuf1, tbuf2);
    *msg_out = "Bad valid-after time";
    goto err;
  }

  /* Fetch any new router descriptors we just learned about */
  update_consensus_router_descriptor_downloads(time(NULL), 1, vote);

  /* Now see whether we already have a vote from this authority. */
  SMARTLIST_FOREACH_BEGIN(pending_vote_list, pending_vote_t *, v) {
      if (fast_memeq(v->vote->cert->cache_info.identity_digest,
                   vote->cert->cache_info.identity_digest,
                   DIGEST_LEN)) {
        networkstatus_voter_info_t *vi_old = get_voter(v->vote);
        if (fast_memeq(vi_old->vote_digest, vi->vote_digest, DIGEST_LEN)) {
          /* Ah, it's the same vote. Not a problem. */
          log_info(LD_DIR, "Discarding a vote we already have (from %s).",
                   vi->address);
          if (*status_out < 200)
            *status_out = 200;
          goto discard;
        } else if (v->vote->published < vote->published) {
          log_notice(LD_DIR, "Replacing an older pending vote from this "
                     "directory (%s)", vi->address);
          cached_dir_decref(v->vote_body);
          networkstatus_vote_free(v->vote);
          v->vote_body = new_cached_dir(tor_strndup(vote_body,
                                                    end_of_vote-vote_body),
                                        vote->published);
          v->vote = vote;
          if (end_of_vote &&
              !strcmpstart(end_of_vote, "network-status-version"))
            goto again;

          if (*status_out < 200)
            *status_out = 200;
          if (!*msg_out)
            *msg_out = "OK";
          return v;
        } else {
          *msg_out = "Already have a newer pending vote";
          goto err;
        }
      }
  } SMARTLIST_FOREACH_END(v);

  /* This a valid vote, update our shared random state. */
  sr_handle_received_commits(vote->sr_info.commits,
                             vote->cert->identity_key);

  pending_vote = tor_malloc_zero(sizeof(pending_vote_t));
  pending_vote->vote_body = new_cached_dir(tor_strndup(vote_body,
                                                       end_of_vote-vote_body),
                                           vote->published);
  pending_vote->vote = vote;
  smartlist_add(pending_vote_list, pending_vote);

  if (!strcmpstart(end_of_vote, "network-status-version ")) {
    vote_body = end_of_vote;
    goto again;
  }

  goto done;

 err:
  any_failed = 1;
  if (!*msg_out)
    *msg_out = "Error adding vote";
  if (*status_out < 400)
    *status_out = 400;

 discard:
  networkstatus_vote_free(vote);

  if (end_of_vote && !strcmpstart(end_of_vote, "network-status-version ")) {
    vote_body = end_of_vote;
    goto again;
  }

 done:

  if (*status_out < 200)
    *status_out = 200;
  if (!*msg_out) {
    if (!any_failed && !pending_vote) {
      *msg_out = "Duplicate discarded";
    } else {
      *msg_out = "ok";
    }
  }

  return any_failed ? NULL : pending_vote;
}