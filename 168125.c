get_voter(const networkstatus_t *vote)
{
  tor_assert(vote);
  tor_assert(vote->type == NS_TYPE_VOTE);
  tor_assert(vote->voters);
  tor_assert(smartlist_len(vote->voters) == 1);
  return smartlist_get(vote->voters, 0);
}