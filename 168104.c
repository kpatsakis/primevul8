dirvote_format_all_microdesc_vote_lines(const routerinfo_t *ri, time_t now,
                                        smartlist_t *microdescriptors_out)
{
  const struct consensus_method_range_t *cmr;
  microdesc_vote_line_t *entries = NULL, *ep;
  vote_microdesc_hash_t *result = NULL;

  /* Generate the microdescriptors. */
  for (cmr = microdesc_consensus_methods;
       cmr->low != -1 && cmr->high != -1;
       cmr++) {
    microdesc_t *md = dirvote_create_microdescriptor(ri, cmr->low);
    if (md) {
      microdesc_vote_line_t *e =
        tor_malloc_zero(sizeof(microdesc_vote_line_t));
      e->md = md;
      e->low = cmr->low;
      e->high = cmr->high;
      e->next = entries;
      entries = e;
    }
  }

  /* Compress adjacent identical ones */
  for (ep = entries; ep; ep = ep->next) {
    while (ep->next &&
           fast_memeq(ep->md->digest, ep->next->md->digest, DIGEST256_LEN) &&
           ep->low == ep->next->high + 1) {
      microdesc_vote_line_t *next = ep->next;
      ep->low = next->low;
      microdesc_free(next->md);
      ep->next = next->next;
      tor_free(next);
    }
  }

  /* Format them into vote_microdesc_hash_t, and add to microdescriptors_out.*/
  while ((ep = entries)) {
    char buf[128];
    vote_microdesc_hash_t *h;
    dirvote_format_microdesc_vote_line(buf, sizeof(buf), ep->md,
                                       ep->low, ep->high);
    h = tor_malloc_zero(sizeof(vote_microdesc_hash_t));
    h->microdesc_hash_line = tor_strdup(buf);
    h->next = result;
    result = h;
    ep->md->last_listed = now;
    smartlist_add(microdescriptors_out, ep->md);
    entries = ep->next;
    tor_free(ep);
  }

  return result;
}