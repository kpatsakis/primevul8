compute_consensus_method(smartlist_t *votes)
{
  smartlist_t *all_methods = smartlist_new();
  smartlist_t *acceptable_methods = smartlist_new();
  smartlist_t *tmp = smartlist_new();
  int min = (smartlist_len(votes) * 2) / 3;
  int n_ok;
  int result;
  SMARTLIST_FOREACH(votes, networkstatus_t *, vote,
  {
    tor_assert(vote->supported_methods);
    smartlist_add_all(tmp, vote->supported_methods);
    smartlist_sort(tmp, cmp_int_strings_);
    smartlist_uniq(tmp, cmp_int_strings_, NULL);
    smartlist_add_all(all_methods, tmp);
    smartlist_clear(tmp);
  });

  smartlist_sort(all_methods, cmp_int_strings_);
  get_frequent_members(acceptable_methods, all_methods, min);
  n_ok = smartlist_len(acceptable_methods);
  if (n_ok) {
    const char *best = smartlist_get(acceptable_methods, n_ok-1);
    result = (int)tor_parse_long(best, 10, 1, INT_MAX, NULL, NULL);
  } else {
    result = 1;
  }
  smartlist_free(tmp);
  smartlist_free(all_methods);
  smartlist_free(acceptable_methods);
  return result;
}