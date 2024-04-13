compare_dir_src_ents_by_authority_id_(const void **_a, const void **_b)
{
  const dir_src_ent_t *a = *_a, *b = *_b;
  const networkstatus_voter_info_t *a_v = get_voter(a->v),
    *b_v = get_voter(b->v);
  const char *a_id, *b_id;
  a_id = a->is_legacy ? a_v->legacy_id_digest : a_v->identity_digest;
  b_id = b->is_legacy ? b_v->legacy_id_digest : b_v->identity_digest;

  return fast_memcmp(a_id, b_id, DIGEST_LEN);
}