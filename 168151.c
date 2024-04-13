format_networkstatus_vote(crypto_pk_t *private_signing_key,
                          networkstatus_t *v3_ns)
{
  smartlist_t *chunks = smartlist_new();
  char *packages = NULL;
  char fingerprint[FINGERPRINT_LEN+1];
  char digest[DIGEST_LEN];
  uint32_t addr;
  char *protocols_lines = NULL;
  char *client_versions_line = NULL, *server_versions_line = NULL;
  char *shared_random_vote_str = NULL;
  networkstatus_voter_info_t *voter;
  char *status = NULL;

  tor_assert(private_signing_key);
  tor_assert(v3_ns->type == NS_TYPE_VOTE || v3_ns->type == NS_TYPE_OPINION);

  voter = smartlist_get(v3_ns->voters, 0);

  addr = voter->addr;

  base16_encode(fingerprint, sizeof(fingerprint),
                v3_ns->cert->cache_info.identity_digest, DIGEST_LEN);

  client_versions_line = format_line_if_present("client-versions",
                                                v3_ns->client_versions);
  server_versions_line = format_line_if_present("server-versions",
                                                v3_ns->server_versions);
  protocols_lines = format_protocols_lines_for_vote(v3_ns);

  if (v3_ns->package_lines) {
    smartlist_t *tmp = smartlist_new();
    SMARTLIST_FOREACH(v3_ns->package_lines, const char *, p,
                      if (validate_recommended_package_line(p))
                        smartlist_add_asprintf(tmp, "package %s\n", p));
    smartlist_sort_strings(tmp);
    packages = smartlist_join_strings(tmp, "", 0, NULL);
    SMARTLIST_FOREACH(tmp, char *, cp, tor_free(cp));
    smartlist_free(tmp);
  } else {
    packages = tor_strdup("");
  }

    /* Get shared random commitments/reveals line(s). */
  shared_random_vote_str = sr_get_string_for_vote();

  {
    char published[ISO_TIME_LEN+1];
    char va[ISO_TIME_LEN+1];
    char fu[ISO_TIME_LEN+1];
    char vu[ISO_TIME_LEN+1];
    char *flags = smartlist_join_strings(v3_ns->known_flags, " ", 0, NULL);
    /* XXXX Abstraction violation: should be pulling a field out of v3_ns.*/
    char *flag_thresholds = dirserv_get_flag_thresholds_line();
    char *params;
    authority_cert_t *cert = v3_ns->cert;
    char *methods =
      make_consensus_method_list(MIN_SUPPORTED_CONSENSUS_METHOD,
                                 MAX_SUPPORTED_CONSENSUS_METHOD, " ");
    format_iso_time(published, v3_ns->published);
    format_iso_time(va, v3_ns->valid_after);
    format_iso_time(fu, v3_ns->fresh_until);
    format_iso_time(vu, v3_ns->valid_until);

    if (v3_ns->net_params)
      params = smartlist_join_strings(v3_ns->net_params, " ", 0, NULL);
    else
      params = tor_strdup("");

    tor_assert(cert);
    smartlist_add_asprintf(chunks,
                 "network-status-version 3\n"
                 "vote-status %s\n"
                 "consensus-methods %s\n"
                 "published %s\n"
                 "valid-after %s\n"
                 "fresh-until %s\n"
                 "valid-until %s\n"
                 "voting-delay %d %d\n"
                 "%s%s" /* versions */
                 "%s" /* protocols */
                 "%s" /* packages */
                 "known-flags %s\n"
                 "flag-thresholds %s\n"
                 "params %s\n"
                 "dir-source %s %s %s %s %d %d\n"
                 "contact %s\n"
                 "%s", /* shared randomness information */
                 v3_ns->type == NS_TYPE_VOTE ? "vote" : "opinion",
                 methods,
                 published, va, fu, vu,
                 v3_ns->vote_seconds, v3_ns->dist_seconds,
                 client_versions_line,
                 server_versions_line,
                 protocols_lines,
                 packages,
                 flags,
                 flag_thresholds,
                 params,
                 voter->nickname, fingerprint, voter->address,
                 fmt_addr32(addr), voter->dir_port, voter->or_port,
                 voter->contact,
                 shared_random_vote_str ?
                           shared_random_vote_str : "");

    tor_free(params);
    tor_free(flags);
    tor_free(flag_thresholds);
    tor_free(methods);
    tor_free(shared_random_vote_str);

    if (!tor_digest_is_zero(voter->legacy_id_digest)) {
      char fpbuf[HEX_DIGEST_LEN+1];
      base16_encode(fpbuf, sizeof(fpbuf), voter->legacy_id_digest, DIGEST_LEN);
      smartlist_add_asprintf(chunks, "legacy-dir-key %s\n", fpbuf);
    }

    smartlist_add(chunks, tor_strndup(cert->cache_info.signed_descriptor_body,
                                      cert->cache_info.signed_descriptor_len));
  }

  SMARTLIST_FOREACH_BEGIN(v3_ns->routerstatus_list, vote_routerstatus_t *,
                          vrs) {
    char *rsf;
    vote_microdesc_hash_t *h;
    rsf = routerstatus_format_entry(&vrs->status,
                                    vrs->version, vrs->protocols,
                                    NS_V3_VOTE, vrs);
    if (rsf)
      smartlist_add(chunks, rsf);

    for (h = vrs->microdesc; h; h = h->next) {
      smartlist_add(chunks, tor_strdup(h->microdesc_hash_line));
    }
  } SMARTLIST_FOREACH_END(vrs);

  smartlist_add(chunks, tor_strdup("directory-footer\n"));

  /* The digest includes everything up through the space after
   * directory-signature.  (Yuck.) */
  crypto_digest_smartlist(digest, DIGEST_LEN, chunks,
                          "directory-signature ", DIGEST_SHA1);

  {
    char signing_key_fingerprint[FINGERPRINT_LEN+1];
    if (crypto_pk_get_fingerprint(private_signing_key,
                                  signing_key_fingerprint, 0)<0) {
      log_warn(LD_BUG, "Unable to get fingerprint for signing key");
      goto err;
    }

    smartlist_add_asprintf(chunks, "directory-signature %s %s\n", fingerprint,
                           signing_key_fingerprint);
  }

  note_crypto_pk_op(SIGN_DIR);
  {
    char *sig = router_get_dirobj_signature(digest, DIGEST_LEN,
                                            private_signing_key);
    if (!sig) {
      log_warn(LD_BUG, "Unable to sign networkstatus vote.");
      goto err;
    }
    smartlist_add(chunks, sig);
  }

  status = smartlist_join_strings(chunks, "", 0, NULL);

  {
    networkstatus_t *v;
    if (!(v = networkstatus_parse_vote_from_string(status, NULL,
                                                   v3_ns->type))) {
      log_err(LD_BUG,"Generated a networkstatus %s we couldn't parse: "
              "<<%s>>",
              v3_ns->type == NS_TYPE_VOTE ? "vote" : "opinion", status);
      goto err;
    }
    networkstatus_vote_free(v);
  }

  goto done;

 err:
  tor_free(status);
 done:
  tor_free(client_versions_line);
  tor_free(server_versions_line);
  tor_free(protocols_lines);
  tor_free(packages);

  SMARTLIST_FOREACH(chunks, char *, cp, tor_free(cp));
  smartlist_free(chunks);
  return status;
}