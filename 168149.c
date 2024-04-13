compute_routerstatus_consensus(smartlist_t *votes, int consensus_method,
                               char *microdesc_digest256_out,
                               tor_addr_port_t *best_alt_orport_out)
{
  vote_routerstatus_t *most = NULL, *cur = NULL;
  int most_n = 0, cur_n = 0;
  time_t most_published = 0;

  /* compare_vote_rs_() sorts the items by identity digest (all the same),
   * then by SD digest.  That way, if we have a tie that the published_on
   * date cannot tie, we use the descriptor with the smaller digest.
   */
  smartlist_sort(votes, compare_vote_rs_);
  SMARTLIST_FOREACH_BEGIN(votes, vote_routerstatus_t *, rs) {
    if (cur && !compare_vote_rs(cur, rs)) {
      ++cur_n;
    } else {
      if (cur && (cur_n > most_n ||
                  (cur_n == most_n &&
                   cur->status.published_on > most_published))) {
        most = cur;
        most_n = cur_n;
        most_published = cur->status.published_on;
      }
      cur_n = 1;
      cur = rs;
    }
  } SMARTLIST_FOREACH_END(rs);

  if (cur_n > most_n ||
      (cur && cur_n == most_n && cur->status.published_on > most_published)) {
    most = cur;
    most_n = cur_n;
    most_published = cur->status.published_on;
  }

  tor_assert(most);

  /* If we're producing "a" lines, vote on potential alternative (sets
   * of) OR port(s) in the winning routerstatuses.
   *
   * XXX prop186 There's at most one alternative OR port (_the_ IPv6
   * port) for now. */
  if (consensus_method >= MIN_METHOD_FOR_A_LINES && best_alt_orport_out) {
    smartlist_t *alt_orports = smartlist_new();
    const tor_addr_port_t *most_alt_orport = NULL;

    SMARTLIST_FOREACH_BEGIN(votes, vote_routerstatus_t *, rs) {
      tor_assert(rs);
      if (compare_vote_rs(most, rs) == 0 &&
          !tor_addr_is_null(&rs->status.ipv6_addr)
          && rs->status.ipv6_orport) {
        smartlist_add(alt_orports, tor_addr_port_new(&rs->status.ipv6_addr,
                                                     rs->status.ipv6_orport));
      }
    } SMARTLIST_FOREACH_END(rs);

    smartlist_sort(alt_orports, compare_orports_);
    most_alt_orport = smartlist_get_most_frequent(alt_orports,
                                                  compare_orports_);
    if (most_alt_orport) {
      memcpy(best_alt_orport_out, most_alt_orport, sizeof(tor_addr_port_t));
      log_debug(LD_DIR, "\"a\" line winner for %s is %s",
                most->status.nickname,
                fmt_addrport(&most_alt_orport->addr, most_alt_orport->port));
    }

    SMARTLIST_FOREACH(alt_orports, tor_addr_port_t *, ap, tor_free(ap));
    smartlist_free(alt_orports);
  }

  if (microdesc_digest256_out) {
    smartlist_t *digests = smartlist_new();
    const uint8_t *best_microdesc_digest;
    SMARTLIST_FOREACH_BEGIN(votes, vote_routerstatus_t *, rs) {
        char d[DIGEST256_LEN];
        if (compare_vote_rs(rs, most))
          continue;
        if (!vote_routerstatus_find_microdesc_hash(d, rs, consensus_method,
                                                   DIGEST_SHA256))
          smartlist_add(digests, tor_memdup(d, sizeof(d)));
    } SMARTLIST_FOREACH_END(rs);
    smartlist_sort_digests256(digests);
    best_microdesc_digest = smartlist_get_most_frequent_digest256(digests);
    if (best_microdesc_digest)
      memcpy(microdesc_digest256_out, best_microdesc_digest, DIGEST256_LEN);
    SMARTLIST_FOREACH(digests, char *, cp, tor_free(cp));
    smartlist_free(digests);
  }

  return most;
}