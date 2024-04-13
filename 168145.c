compare_votes_by_authority_id_(const void **_a, const void **_b)
{
  const networkstatus_t *a = *_a, *b = *_b;
  return fast_memcmp(get_voter(a)->identity_digest,
                get_voter(b)->identity_digest, DIGEST_LEN);
}