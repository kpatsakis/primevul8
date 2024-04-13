compute_consensus_versions_list(smartlist_t *lst, int n_versioning)
{
  int min = n_versioning / 2;
  smartlist_t *good = smartlist_new();
  char *result;
  sort_version_list(lst, 0);
  get_frequent_members(good, lst, min);
  result = smartlist_join_strings(good, ",", 0, NULL);
  smartlist_free(good);
  return result;
}