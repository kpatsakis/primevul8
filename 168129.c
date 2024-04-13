dirvote_compute_consensuses(void)
{
  /* Have we got enough votes to try? */
  int n_votes, n_voters, n_vote_running = 0;
  smartlist_t *votes = NULL;
  char *consensus_body = NULL, *signatures = NULL;
  networkstatus_t *consensus = NULL;
  authority_cert_t *my_cert;
  pending_consensus_t pending[N_CONSENSUS_FLAVORS];
  int flav;

  memset(pending, 0, sizeof(pending));

  if (!pending_vote_list)
    pending_vote_list = smartlist_new();

  /* Write votes to disk */
  write_v3_votes_to_disk(pending_vote_list);

  /* Setup votes smartlist */
  votes = smartlist_new();
  SMARTLIST_FOREACH(pending_vote_list, pending_vote_t *, v,
    {
      smartlist_add(votes, v->vote); /* collect votes to compute consensus */
    });

  /* See if consensus managed to achieve majority */
  n_voters = get_n_authorities(V3_DIRINFO);
  n_votes = smartlist_len(pending_vote_list);
  if (n_votes <= n_voters/2) {
    log_warn(LD_DIR, "We don't have enough votes to generate a consensus: "
             "%d of %d", n_votes, n_voters/2+1);
    goto err;
  }
  tor_assert(pending_vote_list);
  SMARTLIST_FOREACH(pending_vote_list, pending_vote_t *, v, {
    if (smartlist_contains_string(v->vote->known_flags, "Running"))
      n_vote_running++;
  });
  if (!n_vote_running) {
    /* See task 1066. */
    log_warn(LD_DIR, "Nobody has voted on the Running flag. Generating "
                     "and publishing a consensus without Running nodes "
                     "would make many clients stop working. Not "
                     "generating a consensus!");
    goto err;
  }

  if (!(my_cert = get_my_v3_authority_cert())) {
    log_warn(LD_DIR, "Can't generate consensus without a certificate.");
    goto err;
  }

  {
    char legacy_dbuf[DIGEST_LEN];
    crypto_pk_t *legacy_sign=NULL;
    char *legacy_id_digest = NULL;
    int n_generated = 0;
    if (get_options()->V3AuthUseLegacyKey) {
      authority_cert_t *cert = get_my_v3_legacy_cert();
      legacy_sign = get_my_v3_legacy_signing_key();
      if (cert) {
        if (crypto_pk_get_digest(cert->identity_key, legacy_dbuf)) {
          log_warn(LD_BUG,
                   "Unable to compute digest of legacy v3 identity key");
        } else {
          legacy_id_digest = legacy_dbuf;
        }
      }
    }

    for (flav = 0; flav < N_CONSENSUS_FLAVORS; ++flav) {
      const char *flavor_name = networkstatus_get_flavor_name(flav);
      consensus_body = networkstatus_compute_consensus(
        votes, n_voters,
        my_cert->identity_key,
        get_my_v3_authority_signing_key(), legacy_id_digest, legacy_sign,
        flav);

      if (!consensus_body) {
        log_warn(LD_DIR, "Couldn't generate a %s consensus at all!",
                 flavor_name);
        continue;
      }
      consensus = networkstatus_parse_vote_from_string(consensus_body, NULL,
                                                       NS_TYPE_CONSENSUS);
      if (!consensus) {
        log_warn(LD_DIR, "Couldn't parse %s consensus we generated!",
                 flavor_name);
        tor_free(consensus_body);
        continue;
      }

      /* 'Check' our own signature, to mark it valid. */
      networkstatus_check_consensus_signature(consensus, -1);

      pending[flav].body = consensus_body;
      pending[flav].consensus = consensus;
      n_generated++;
      consensus_body = NULL;
      consensus = NULL;
    }
    if (!n_generated) {
      log_warn(LD_DIR, "Couldn't generate any consensus flavors at all.");
      goto err;
    }
  }

  signatures = get_detached_signatures_from_pending_consensuses(
       pending, N_CONSENSUS_FLAVORS);

  if (!signatures) {
    log_warn(LD_DIR, "Couldn't extract signatures.");
    goto err;
  }

  dirvote_clear_pending_consensuses();
  memcpy(pending_consensuses, pending, sizeof(pending));

  tor_free(pending_consensus_signatures);
  pending_consensus_signatures = signatures;

  if (pending_consensus_signature_list) {
    int n_sigs = 0;
    /* we may have gotten signatures for this consensus before we built
     * it ourself.  Add them now. */
    SMARTLIST_FOREACH_BEGIN(pending_consensus_signature_list, char *, sig) {
        const char *msg = NULL;
        int r = dirvote_add_signatures_to_all_pending_consensuses(sig,
                                                     "pending", &msg);
        if (r >= 0)
          n_sigs += r;
        else
          log_warn(LD_DIR,
                   "Could not add queued signature to new consensus: %s",
                   msg);
        tor_free(sig);
    } SMARTLIST_FOREACH_END(sig);
    if (n_sigs)
      log_notice(LD_DIR, "Added %d pending signatures while building "
                 "consensus.", n_sigs);
    smartlist_clear(pending_consensus_signature_list);
  }

  log_notice(LD_DIR, "Consensus computed; uploading signature(s)");

  directory_post_to_dirservers(DIR_PURPOSE_UPLOAD_SIGNATURES,
                               ROUTER_PURPOSE_GENERAL,
                               V3_DIRINFO,
                               pending_consensus_signatures,
                               strlen(pending_consensus_signatures), 0);
  log_notice(LD_DIR, "Signature(s) posted.");

  smartlist_free(votes);
  return 0;
 err:
  smartlist_free(votes);
  tor_free(consensus_body);
  tor_free(signatures);
  networkstatus_vote_free(consensus);

  return -1;
}