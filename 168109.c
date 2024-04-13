networkstatus_get_detached_signatures(smartlist_t *consensuses)
{
  smartlist_t *elements;
  char *result = NULL, *sigs = NULL;
  networkstatus_t *consensus_ns = NULL;
  tor_assert(consensuses);

  SMARTLIST_FOREACH(consensuses, networkstatus_t *, ns, {
      tor_assert(ns);
      tor_assert(ns->type == NS_TYPE_CONSENSUS);
      if (ns && ns->flavor == FLAV_NS)
        consensus_ns = ns;
  });
  if (!consensus_ns) {
    log_warn(LD_BUG, "No NS consensus given.");
    return NULL;
  }

  elements = smartlist_new();

  {
    char va_buf[ISO_TIME_LEN+1], fu_buf[ISO_TIME_LEN+1],
      vu_buf[ISO_TIME_LEN+1];
    char d[HEX_DIGEST_LEN+1];

    base16_encode(d, sizeof(d),
                  consensus_ns->digests.d[DIGEST_SHA1], DIGEST_LEN);
    format_iso_time(va_buf, consensus_ns->valid_after);
    format_iso_time(fu_buf, consensus_ns->fresh_until);
    format_iso_time(vu_buf, consensus_ns->valid_until);

    smartlist_add_asprintf(elements,
                 "consensus-digest %s\n"
                 "valid-after %s\n"
                 "fresh-until %s\n"
                 "valid-until %s\n", d, va_buf, fu_buf, vu_buf);
  }

  /* Get all the digests for the non-FLAV_NS consensuses */
  SMARTLIST_FOREACH_BEGIN(consensuses, networkstatus_t *, ns) {
    const char *flavor_name = networkstatus_get_flavor_name(ns->flavor);
    int alg;
    if (ns->flavor == FLAV_NS)
      continue;

    /* start with SHA256; we don't include SHA1 for anything but the basic
     * consensus. */
    for (alg = DIGEST_SHA256; alg < N_COMMON_DIGEST_ALGORITHMS; ++alg) {
      char d[HEX_DIGEST256_LEN+1];
      const char *alg_name =
        crypto_digest_algorithm_get_name(alg);
      if (tor_mem_is_zero(ns->digests.d[alg], DIGEST256_LEN))
        continue;
      base16_encode(d, sizeof(d), ns->digests.d[alg], DIGEST256_LEN);
      smartlist_add_asprintf(elements, "additional-digest %s %s %s\n",
                   flavor_name, alg_name, d);
    }
  } SMARTLIST_FOREACH_END(ns);

  /* Now get all the sigs for non-FLAV_NS consensuses */
  SMARTLIST_FOREACH_BEGIN(consensuses, networkstatus_t *, ns) {
    char *sigs_on_this_consensus;
    if (ns->flavor == FLAV_NS)
      continue;
    sigs_on_this_consensus = networkstatus_format_signatures(ns, 1);
    if (!sigs_on_this_consensus) {
      log_warn(LD_DIR, "Couldn't format signatures");
      goto err;
    }
    smartlist_add(elements, sigs_on_this_consensus);
  } SMARTLIST_FOREACH_END(ns);

  /* Now add the FLAV_NS consensus signatrures. */
  sigs = networkstatus_format_signatures(consensus_ns, 1);
  if (!sigs)
    goto err;
  smartlist_add(elements, sigs);

  result = smartlist_join_strings(elements, "", 0, NULL);
 err:
  SMARTLIST_FOREACH(elements, char *, cp, tor_free(cp));
  smartlist_free(elements);
  return result;
}