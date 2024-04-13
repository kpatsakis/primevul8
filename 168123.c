compute_nth_protocol_set(int n, int n_voters, const smartlist_t *votes)
{
  const char *keyword;
  smartlist_t *proto_votes = smartlist_new();
  int threshold;
  switch (n) {
    case 0:
      keyword = "recommended-client-protocols";
      threshold = CEIL_DIV(n_voters, 2);
      break;
    case 1:
      keyword = "recommended-relay-protocols";
      threshold = CEIL_DIV(n_voters, 2);
      break;
    case 2:
      keyword = "required-client-protocols";
      threshold = CEIL_DIV(n_voters * 2, 3);
      break;
    case 3:
      keyword = "required-relay-protocols";
      threshold = CEIL_DIV(n_voters * 2, 3);
      break;
    default:
      tor_assert_unreached();
      return NULL;
  }

  SMARTLIST_FOREACH_BEGIN(votes, const networkstatus_t *, ns) {
    const char *v = get_nth_protocol_set_vote(n, ns);
    if (v)
      smartlist_add(proto_votes, (void*)v);
  } SMARTLIST_FOREACH_END(ns);

  char *protocols = protover_compute_vote(proto_votes, threshold);
  smartlist_free(proto_votes);

  char *result = NULL;
  tor_asprintf(&result, "%s %s\n", keyword, protocols);
  tor_free(protocols);

  return result;
}