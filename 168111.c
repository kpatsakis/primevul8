dirvote_compute_params(smartlist_t *votes, int method, int total_authorities)
{
  int i;
  int32_t *vals;

  int cur_param_len;
  const char *cur_param;
  const char *eq;

  const int n_votes = smartlist_len(votes);
  smartlist_t *output;
  smartlist_t *param_list = smartlist_new();
  (void) method;

  /* We require that the parameter lists in the votes are well-formed: that
     is, that their keywords are unique and sorted, and that their values are
     between INT32_MIN and INT32_MAX inclusive.  This should be guaranteed by
     the parsing code. */

  vals = tor_calloc(n_votes, sizeof(int));

  SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
    if (!v->net_params)
      continue;
    smartlist_add_all(param_list, v->net_params);
  } SMARTLIST_FOREACH_END(v);

  if (smartlist_len(param_list) == 0) {
    tor_free(vals);
    return param_list;
  }

  smartlist_sort_strings(param_list);
  i = 0;
  cur_param = smartlist_get(param_list, 0);
  eq = strchr(cur_param, '=');
  tor_assert(eq);
  cur_param_len = (int)(eq+1 - cur_param);

  output = smartlist_new();

  SMARTLIST_FOREACH_BEGIN(param_list, const char *, param) {
    const char *next_param;
    int ok=0;
    eq = strchr(param, '=');
    tor_assert(i<n_votes); /* Make sure we prevented vote-stuffing. */
    vals[i++] = (int32_t)
      tor_parse_long(eq+1, 10, INT32_MIN, INT32_MAX, &ok, NULL);
    tor_assert(ok); /* Already checked these when parsing. */

    if (param_sl_idx+1 == smartlist_len(param_list))
      next_param = NULL;
    else
      next_param = smartlist_get(param_list, param_sl_idx+1);
    /* resolve spurious clang shallow analysis null pointer errors */
    tor_assert(param);
    if (!next_param || strncmp(next_param, param, cur_param_len)) {
      /* We've reached the end of a series. */
      /* Make sure enough authorities voted on this param, unless the
       * the consensus method we use is too old for that. */
      if (i > total_authorities/2 ||
          i >= MIN_VOTES_FOR_PARAM) {
        int32_t median = median_int32(vals, i);
        char *out_string = tor_malloc(64+cur_param_len);
        memcpy(out_string, param, cur_param_len);
        tor_snprintf(out_string+cur_param_len,64, "%ld", (long)median);
        smartlist_add(output, out_string);
      }

      i = 0;
      if (next_param) {
        eq = strchr(next_param, '=');
        cur_param_len = (int)(eq+1 - next_param);
      }
    }
  } SMARTLIST_FOREACH_END(param);

  smartlist_free(param_list);
  tor_free(vals);
  return output;
}