networkstatus_add_detached_signatures(networkstatus_t *target,
                                      ns_detached_signatures_t *sigs,
                                      const char *source,
                                      int severity,
                                      const char **msg_out)
{
  int r = 0;
  const char *flavor;
  smartlist_t *siglist;
  tor_assert(sigs);
  tor_assert(target);
  tor_assert(target->type == NS_TYPE_CONSENSUS);

  flavor = networkstatus_get_flavor_name(target->flavor);

  /* Do the times seem right? */
  if (target->valid_after != sigs->valid_after) {
    *msg_out = "Valid-After times do not match "
      "when adding detached signatures to consensus";
    return -1;
  }
  if (target->fresh_until != sigs->fresh_until) {
    *msg_out = "Fresh-until times do not match "
      "when adding detached signatures to consensus";
    return -1;
  }
  if (target->valid_until != sigs->valid_until) {
    *msg_out = "Valid-until times do not match "
      "when adding detached signatures to consensus";
    return -1;
  }
  siglist = strmap_get(sigs->signatures, flavor);
  if (!siglist) {
    *msg_out = "No signatures for given consensus flavor";
    return -1;
  }

  /** Make sure all the digests we know match, and at least one matches. */
  {
    common_digests_t *digests = strmap_get(sigs->digests, flavor);
    int n_matches = 0;
    int alg;
    if (!digests) {
      *msg_out = "No digests for given consensus flavor";
      return -1;
    }
    for (alg = DIGEST_SHA1; alg < N_COMMON_DIGEST_ALGORITHMS; ++alg) {
      if (!tor_mem_is_zero(digests->d[alg], DIGEST256_LEN)) {
        if (fast_memeq(target->digests.d[alg], digests->d[alg],
                       DIGEST256_LEN)) {
          ++n_matches;
        } else {
          *msg_out = "Mismatched digest.";
          return -1;
        }
      }
    }
    if (!n_matches) {
      *msg_out = "No recognized digests for given consensus flavor";
    }
  }

  /* For each voter in src... */
  SMARTLIST_FOREACH_BEGIN(siglist, document_signature_t *, sig) {
    char voter_identity[HEX_DIGEST_LEN+1];
    networkstatus_voter_info_t *target_voter =
      networkstatus_get_voter_by_id(target, sig->identity_digest);
    authority_cert_t *cert = NULL;
    const char *algorithm;
    document_signature_t *old_sig = NULL;

    algorithm = crypto_digest_algorithm_get_name(sig->alg);

    base16_encode(voter_identity, sizeof(voter_identity),
                  sig->identity_digest, DIGEST_LEN);
    log_info(LD_DIR, "Looking at signature from %s using %s", voter_identity,
             algorithm);
    /* If the target doesn't know about this voter, then forget it. */
    if (!target_voter) {
      log_info(LD_DIR, "We do not know any voter with ID %s", voter_identity);
      continue;
    }

    old_sig = voter_get_sig_by_algorithm(target_voter, sig->alg);

    /* If the target already has a good signature from this voter, then skip
     * this one. */
    if (old_sig && old_sig->good_signature) {
      log_info(LD_DIR, "We already have a good signature from %s using %s",
               voter_identity, algorithm);
      continue;
    }

    /* Try checking the signature if we haven't already. */
    if (!sig->good_signature && !sig->bad_signature) {
      cert = authority_cert_get_by_digests(sig->identity_digest,
                                           sig->signing_key_digest);
      if (cert) {
        /* Not checking the return value here, since we are going to look
         * at the status of sig->good_signature in a moment. */
        (void) networkstatus_check_document_signature(target, sig, cert);
      }
    }

    /* If this signature is good, or we don't have any signature yet,
     * then maybe add it. */
    if (sig->good_signature || !old_sig || old_sig->bad_signature) {
      log_info(LD_DIR, "Adding signature from %s with %s", voter_identity,
               algorithm);
      tor_log(severity, LD_DIR, "Added a signature for %s from %s.",
          target_voter->nickname, source);
      ++r;
      if (old_sig) {
        smartlist_remove(target_voter->sigs, old_sig);
        document_signature_free(old_sig);
      }
      smartlist_add(target_voter->sigs, document_signature_dup(sig));
    } else {
      log_info(LD_DIR, "Not adding signature from %s", voter_identity);
    }
  } SMARTLIST_FOREACH_END(sig);

  return r;
}