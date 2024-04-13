make_consensus_method_list(int low, int high, const char *separator)
{
  char *list;

  int i;
  smartlist_t *lst;
  lst = smartlist_new();
  for (i = low; i <= high; ++i) {
    if (!consensus_method_is_supported(i))
      continue;
    smartlist_add_asprintf(lst, "%d", i);
  }
  list = smartlist_join_strings(lst, separator, 0, NULL);
  tor_assert(list);
  SMARTLIST_FOREACH(lst, char *, cp, tor_free(cp));
  smartlist_free(lst);
  return list;
}