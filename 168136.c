get_frequent_members(smartlist_t *out, smartlist_t *in, int min)
{
  char *cur = NULL;
  int count = 0;
  SMARTLIST_FOREACH_BEGIN(in, char *, cp) {
    if (cur && !strcmp(cp, cur)) {
      ++count;
    } else {
      if (count > min)
        smartlist_add(out, cur);
      cur = cp;
      count = 1;
    }
  } SMARTLIST_FOREACH_END(cp);
  if (count > min)
    smartlist_add(out, cur);
}