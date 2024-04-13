onig_regset_search_with_param(OnigRegSet* set,
           const UChar* str, const UChar* end,
           const UChar* start, const UChar* range,
           OnigRegSetLead lead, OnigOptionType option, OnigMatchParam* mps[],
           int* rmatch_pos)
{
  int r;
  int i;
  UChar *s, *prev;
  regex_t* reg;
  OnigEncoding enc;
  OnigRegion* region;
  MatchArg* msas;
  const UChar *orig_start = start;
  const UChar *orig_range = range;

  if (set->n == 0)
    return ONIG_MISMATCH;

  if (IS_POSIX_REGION(option))
    return ONIGERR_INVALID_ARGUMENT;

  r = 0;
  enc = set->enc;
  msas = (MatchArg* )NULL;

  for (i = 0; i < set->n; i++) {
    reg    = set->rs[i].reg;
    region = set->rs[i].region;
    ADJUST_MATCH_PARAM(reg, mps[i]);
    if (IS_NOT_NULL(region)) {
      r = onig_region_resize_clear(region, reg->num_mem + 1);
      if (r != 0) goto finish_no_msa;
    }
  }

  if (start > end || start < str) goto mismatch_no_msa;
  if (str < end) {
    /* forward search only */
    if (range <= start)
      return ONIGERR_INVALID_ARGUMENT;
  }

  if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
    if (! ONIGENC_IS_VALID_MBC_STRING(enc, str, end)) {
      r = ONIGERR_INVALID_WIDE_CHAR_VALUE;
      goto finish_no_msa;
    }
  }

  if (set->anchor != OPTIMIZE_NONE && str < end) {
    UChar *min_semi_end, *max_semi_end;

    if ((set->anchor & ANCR_BEGIN_POSITION) != 0) {
      /* search start-position only */
    begin_position:
      range = start + 1;
    }
    else if ((set->anchor & ANCR_BEGIN_BUF) != 0) {
      /* search str-position only */
      if (start != str) goto mismatch_no_msa;
      range = str + 1;
    }
    else if ((set->anchor & ANCR_END_BUF) != 0) {
      min_semi_end = max_semi_end = (UChar* )end;

    end_buf:
      if ((OnigLen )(max_semi_end - str) < set->anc_dmin)
        goto mismatch_no_msa;

      if ((OnigLen )(min_semi_end - start) > set->anc_dmax) {
        start = min_semi_end - set->anc_dmax;
        if (start < end)
          start = onigenc_get_right_adjust_char_head(enc, str, start);
      }
      if ((OnigLen )(max_semi_end - (range - 1)) < set->anc_dmin) {
        range = max_semi_end - set->anc_dmin + 1;
      }
      if (start > range) goto mismatch_no_msa;
    }
    else if ((set->anchor & ANCR_SEMI_END_BUF) != 0) {
      UChar* pre_end = ONIGENC_STEP_BACK(enc, str, end, 1);

      max_semi_end = (UChar* )end;
      if (ONIGENC_IS_MBC_NEWLINE(enc, pre_end, end)) {
        min_semi_end = pre_end;

#ifdef USE_CRNL_AS_LINE_TERMINATOR
        pre_end = ONIGENC_STEP_BACK(enc, str, pre_end, 1);
        if (IS_NOT_NULL(pre_end) &&
            ONIGENC_IS_MBC_CRNL(enc, pre_end, end)) {
          min_semi_end = pre_end;
        }
#endif
        if (min_semi_end > str && start <= min_semi_end) {
          goto end_buf;
        }
      }
      else {
        min_semi_end = (UChar* )end;
        goto end_buf;
      }
    }
    else if ((set->anchor & ANCR_ANYCHAR_INF_ML) != 0) {
      goto begin_position;
    }
  }
  else if (str == end) { /* empty string */
    start = end = str;
    s = (UChar* )start;
    prev = (UChar* )NULL;

    msas = (MatchArg* )xmalloc(sizeof(*msas) * set->n);
    CHECK_NULL_RETURN_MEMERR(msas);
    for (i = 0; i < set->n; i++) {
      reg = set->rs[i].reg;
      MATCH_ARG_INIT(msas[i], reg, option, set->rs[i].region, start, mps[i]);
    }
    for (i = 0; i < set->n; i++) {
      reg = set->rs[i].reg;
      if (reg->threshold_len == 0) {
        REGSET_MATCH_AND_RETURN_CHECK(end);
      }
    }

    goto mismatch;
  }

  if (lead == ONIG_REGSET_POSITION_LEAD) {
    msas = (MatchArg* )xmalloc(sizeof(*msas) * set->n);
    CHECK_NULL_RETURN_MEMERR(msas);

    for (i = 0; i < set->n; i++) {
      MATCH_ARG_INIT(msas[i], set->rs[i].reg, option, set->rs[i].region,
                     orig_start, mps[i]);
    }

    r = regset_search_body_position_lead(set, str, end, start, range,
                                         orig_range, option, msas, rmatch_pos);
  }
  else {
    r = regset_search_body_regex_lead(set, str, end, start, orig_range,
                                      lead, option, mps, rmatch_pos);
  }
  if (r < 0) goto finish;
  else       goto match2;

 mismatch:
  r = ONIG_MISMATCH;
 finish:
  for (i = 0; i < set->n; i++) {
    if (IS_NOT_NULL(msas))
      MATCH_ARG_FREE(msas[i]);
    if (IS_FIND_NOT_EMPTY(set->rs[i].reg->options) &&
        IS_NOT_NULL(set->rs[i].region)) {
      onig_region_clear(set->rs[i].region);
    }
  }
  if (IS_NOT_NULL(msas)) xfree(msas);
  return r;

 mismatch_no_msa:
  r = ONIG_MISMATCH;
 finish_no_msa:
  return r;

 match:
  *rmatch_pos = (int )(s - str);
 match2:
  for (i = 0; i < set->n; i++) {
    if (IS_NOT_NULL(msas))
      MATCH_ARG_FREE(msas[i]);
    if (IS_FIND_NOT_EMPTY(set->rs[i].reg->options) &&
        IS_NOT_NULL(set->rs[i].region)) {
      onig_region_clear(set->rs[i].region);
    }
  }
  if (IS_NOT_NULL(msas)) xfree(msas);
  return r; /* regex index */
}