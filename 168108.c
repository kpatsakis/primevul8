compare_vote_rs(const vote_routerstatus_t *a, const vote_routerstatus_t *b)
{
  int r;
  tor_assert(a);
  tor_assert(b);

  if ((r = fast_memcmp(a->status.identity_digest, b->status.identity_digest,
                  DIGEST_LEN)))
    return r;
  if ((r = fast_memcmp(a->status.descriptor_digest,
                       b->status.descriptor_digest,
                       DIGEST_LEN)))
    return r;
  /* If we actually reached this point, then the identities and
   * the descriptor digests matched, so somebody is making SHA1 collisions.
   */
#define CMP_FIELD(utype, itype, field) do {                             \
    utype aval = (utype) (itype) a->status.field;                       \
    utype bval = (utype) (itype) b->status.field;                       \
    utype u = bval - aval;                                              \
    itype r2 = (itype) u;                                               \
    if (r2 < 0) {                                                       \
      return -1;                                                        \
    } else if (r2 > 0) {                                                \
      return 1;                                                         \
    }                                                                   \
  } while (0)

  CMP_FIELD(uint64_t, int64_t, published_on);

  if ((r = strcmp(b->status.nickname, a->status.nickname)))
    return r;

  CMP_FIELD(unsigned, int, addr);
  CMP_FIELD(unsigned, int, or_port);
  CMP_FIELD(unsigned, int, dir_port);

  return 0;
}