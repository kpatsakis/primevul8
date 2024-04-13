dirvote_get_vote(const char *fp, int flags)
{
  int by_id = flags & DGV_BY_ID;
  const int include_pending = flags & DGV_INCLUDE_PENDING;
  const int include_previous = flags & DGV_INCLUDE_PREVIOUS;

  if (!pending_vote_list && !previous_vote_list)
    return NULL;
  if (fp == NULL) {
    authority_cert_t *c = get_my_v3_authority_cert();
    if (c) {
      fp = c->cache_info.identity_digest;
      by_id = 1;
    } else
      return NULL;
  }
  if (by_id) {
    if (pending_vote_list && include_pending) {
      SMARTLIST_FOREACH(pending_vote_list, pending_vote_t *, pv,
        if (fast_memeq(get_voter(pv->vote)->identity_digest, fp, DIGEST_LEN))
          return pv->vote_body);
    }
    if (previous_vote_list && include_previous) {
      SMARTLIST_FOREACH(previous_vote_list, pending_vote_t *, pv,
        if (fast_memeq(get_voter(pv->vote)->identity_digest, fp, DIGEST_LEN))
          return pv->vote_body);
    }
  } else {
    if (pending_vote_list && include_pending) {
      SMARTLIST_FOREACH(pending_vote_list, pending_vote_t *, pv,
        if (fast_memeq(pv->vote->digests.d[DIGEST_SHA1], fp, DIGEST_LEN))
          return pv->vote_body);
    }
    if (previous_vote_list && include_previous) {
      SMARTLIST_FOREACH(previous_vote_list, pending_vote_t *, pv,
        if (fast_memeq(pv->vote->digests.d[DIGEST_SHA1], fp, DIGEST_LEN))
          return pv->vote_body);
    }
  }
  return NULL;
}