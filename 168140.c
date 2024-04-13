cmp_int_strings_(const void **_a, const void **_b)
{
  const char *a = *_a, *b = *_b;
  int ai = (int)tor_parse_long(a, 10, 1, INT_MAX, NULL, NULL);
  int bi = (int)tor_parse_long(b, 10, 1, INT_MAX, NULL, NULL);
  if (ai<bi) {
    return -1;
  } else if (ai==bi) {
    if (ai == 0) /* Parsing failed. */
      return strcmp(a, b);
    return 0;
  } else {
    return 1;
  }
}