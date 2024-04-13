dirvote_get_intermediate_param_value(const smartlist_t *param_list,
                                     const char *keyword,
                                     int32_t default_val)
{
  unsigned int n_found = 0;
  int32_t value = default_val;

  SMARTLIST_FOREACH_BEGIN(param_list, const char *, k_v_pair) {
    if (!strcmpstart(k_v_pair, keyword) && k_v_pair[strlen(keyword)] == '=') {
      const char *integer_str = &k_v_pair[strlen(keyword)+1];
      int ok;
      value = (int32_t)
        tor_parse_long(integer_str, 10, INT32_MIN, INT32_MAX, &ok, NULL);
      if (BUG(! ok))
        return default_val;
      ++n_found;
    }
  } SMARTLIST_FOREACH_END(k_v_pair);

  if (n_found == 1)
    return value;
  else if (BUG(n_found > 1))
    return default_val;
  else
    return default_val;
}