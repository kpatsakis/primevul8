onig_search_gpos(regex_t* reg, const UChar* str, const UChar* end,
	    const UChar* global_pos,
	    const UChar* start, const UChar* range, OnigRegion* region, OnigOptionType option)
{
  ptrdiff_t r;
  UChar *s, *prev;
  OnigMatchArg msa;
#ifdef USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE
  const UChar *orig_start = start;
  const UChar *orig_range = range;
#endif

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr,
     "onig_search (entry point): str: %"PRIuPTR" (%p), end: %"PRIuPTR", start: %"PRIuPTR", range: %"PRIuPTR"\n",
     (intptr_t )str, str, end - str, start - str, range - str);
#endif

  if (region) {
    r = onig_region_resize_clear(region, reg->num_mem + 1);
    if (r) goto finish_no_msa;
  }

  if (start > end || start < str) goto mismatch_no_msa;


#ifdef USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE
# ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
#  define MATCH_AND_RETURN_CHECK(upper_range) \
  r = match_at(reg, str, end, (upper_range), s, prev, &msa); \
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      if (! IS_FIND_LONGEST(reg->options)) {\
        goto match;\
      }\
    }\
    else goto finish; /* error */ \
  }
# else
#  define MATCH_AND_RETURN_CHECK(upper_range) \
  r = match_at(reg, str, end, (upper_range), s, prev, &msa); \
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      goto match;\
    }\
    else goto finish; /* error */ \
  }
# endif /* USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE */
#else
# ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
#  define MATCH_AND_RETURN_CHECK(none) \
  r = match_at(reg, str, end, s, prev, &msa);\
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      if (! IS_FIND_LONGEST(reg->options)) {\
        goto match;\
      }\
    }\
    else goto finish; /* error */ \
  }
# else
#  define MATCH_AND_RETURN_CHECK(none) \
  r = match_at(reg, str, end, s, prev, &msa);\
  if (r != ONIG_MISMATCH) {\
    if (r >= 0) {\
      goto match;\
    }\
    else goto finish; /* error */ \
  }
# endif /* USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE */
#endif /* USE_MATCH_RANGE_MUST_BE_INSIDE_OF_SPECIFIED_RANGE */


  /* anchor optimize: resume search range */
  if (reg->anchor != 0 && str < end) {
    UChar *min_semi_end, *max_semi_end;

    if (reg->anchor & ANCHOR_BEGIN_POSITION) {
      /* search start-position only */
    begin_position:
      if (range > start)
      {
	if (global_pos > start)
	{
	  if (global_pos < range)
	    range = global_pos + 1;
	}
	else
	  range = start + 1;
      }
      else
	range = start;
    }
    else if (reg->anchor & ANCHOR_BEGIN_BUF) {
      /* search str-position only */
      if (range > start) {
	if (start != str) goto mismatch_no_msa;
	range = str + 1;
      }
      else {
	if (range <= str) {
	  start = str;
	  range = str;
	}
	else
	  goto mismatch_no_msa;
      }
    }
    else if (reg->anchor & ANCHOR_END_BUF) {
      min_semi_end = max_semi_end = (UChar* )end;

    end_buf:
      if ((OnigDistance )(max_semi_end - str) < reg->anchor_dmin)
	goto mismatch_no_msa;

      if (range > start) {
	if ((OnigDistance )(min_semi_end - start) > reg->anchor_dmax) {
	  start = min_semi_end - reg->anchor_dmax;
	  if (start < end)
	    start = onigenc_get_right_adjust_char_head(reg->enc, str, start, end);
	}
	if ((OnigDistance )(max_semi_end - (range - 1)) < reg->anchor_dmin) {
	  range = max_semi_end - reg->anchor_dmin + 1;
	}

	if (start > range) goto mismatch_no_msa;
	/* If start == range, match with empty at end.
	   Backward search is used. */
      }
      else {
	if ((OnigDistance )(min_semi_end - range) > reg->anchor_dmax) {
	  range = min_semi_end - reg->anchor_dmax;
	}
	if ((OnigDistance )(max_semi_end - start) < reg->anchor_dmin) {
	  start = max_semi_end - reg->anchor_dmin;
	  start = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc, str, start, end);
	}
	if (range > start) goto mismatch_no_msa;
      }
    }
    else if (reg->anchor & ANCHOR_SEMI_END_BUF) {
      UChar* pre_end = ONIGENC_STEP_BACK(reg->enc, str, end, end, 1);

      max_semi_end = (UChar* )end;
      if (ONIGENC_IS_MBC_NEWLINE(reg->enc, pre_end, end)) {
	min_semi_end = pre_end;

#ifdef USE_CRNL_AS_LINE_TERMINATOR
	pre_end = ONIGENC_STEP_BACK(reg->enc, str, pre_end, end, 1);
	if (IS_NOT_NULL(pre_end) &&
	    IS_NEWLINE_CRLF(reg->options) &&
	    ONIGENC_IS_MBC_CRNL(reg->enc, pre_end, end)) {
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
    else if ((reg->anchor & ANCHOR_ANYCHAR_STAR_ML)) {
      goto begin_position;
    }
  }
  else if (str == end) { /* empty string */
    static const UChar address_for_empty_string[] = "";

#ifdef ONIG_DEBUG_SEARCH
    fprintf(stderr, "onig_search: empty string.\n");
#endif

    if (reg->threshold_len == 0) {
      start = end = str = address_for_empty_string;
      s = (UChar* )start;
      prev = (UChar* )NULL;

      MATCH_ARG_INIT(msa, option, region, start, start);
#ifdef USE_COMBINATION_EXPLOSION_CHECK
      msa.state_check_buff = (void* )0;
      msa.state_check_buff_size = 0;   /* NO NEED, for valgrind */
#endif
      MATCH_AND_RETURN_CHECK(end);
      goto mismatch;
    }
    goto mismatch_no_msa;
  }

#ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "onig_search(apply anchor): end: %d, start: %d, range: %d\n",
	  (int )(end - str), (int )(start - str), (int )(range - str));
#endif

  MATCH_ARG_INIT(msa, option, region, start, global_pos);
#ifdef USE_COMBINATION_EXPLOSION_CHECK
  {
    int offset = (MIN(start, range) - str);
    STATE_CHECK_BUFF_INIT(msa, end - str, offset, reg->num_comb_exp_check);
  }
#endif

  s = (UChar* )start;
  if (range > start) {   /* forward search */
    if (s > str)
      prev = onigenc_get_prev_char_head(reg->enc, str, s, end);
    else
      prev = (UChar* )NULL;

    if (reg->optimize != ONIG_OPTIMIZE_NONE) {
      UChar *sch_range, *low, *high, *low_prev;

      sch_range = (UChar* )range;
      if (reg->dmax != 0) {
	if (reg->dmax == ONIG_INFINITE_DISTANCE)
	  sch_range = (UChar* )end;
	else {
	  sch_range += reg->dmax;
	  if (sch_range > end) sch_range = (UChar* )end;
	}
      }

      if ((end - start) < reg->threshold_len)
	goto mismatch;

      if (reg->dmax != ONIG_INFINITE_DISTANCE) {
	do {
	  if (! forward_search_range(reg, str, end, s, sch_range,
				     &low, &high, &low_prev)) goto mismatch;
	  if (s < low) {
	    s    = low;
	    prev = low_prev;
	  }
	  while (s <= high) {
	    MATCH_AND_RETURN_CHECK(orig_range);
	    prev = s;
	    s += enclen(reg->enc, s, end);
	  }
	} while (s < range);
	goto mismatch;
      }
      else { /* check only. */
	if (! forward_search_range(reg, str, end, s, sch_range,
				   &low, &high, (UChar** )NULL)) goto mismatch;

	if ((reg->anchor & ANCHOR_ANYCHAR_STAR) != 0) {
	  do {
	    if ((reg->anchor & ANCHOR_BEGIN_POSITION) == 0)
	      msa.gpos = s;     /* move \G position */
	    MATCH_AND_RETURN_CHECK(orig_range);
	    prev = s;
	    s += enclen(reg->enc, s, end);

	    if ((reg->anchor & (ANCHOR_LOOK_BEHIND | ANCHOR_PREC_READ_NOT)) == 0) {
	      while (!ONIGENC_IS_MBC_NEWLINE_EX(reg->enc, prev, str, end, reg->options, 0)
		  && s < range) {
		prev = s;
		s += enclen(reg->enc, s, end);
	      }
	    }
	  } while (s < range);
	  goto mismatch;
	}
      }
    }

    do {
      MATCH_AND_RETURN_CHECK(orig_range);
      prev = s;
      s += enclen(reg->enc, s, end);
    } while (s < range);

    if (s == range) { /* because empty match with /$/. */
      MATCH_AND_RETURN_CHECK(orig_range);
    }
  }
  else {  /* backward search */
    if (reg->optimize != ONIG_OPTIMIZE_NONE) {
      UChar *low, *high, *adjrange, *sch_start;

      if (range < end)
	adjrange = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc, str, range, end);
      else
	adjrange = (UChar* )end;

      if (reg->dmax != ONIG_INFINITE_DISTANCE &&
	  (end - range) >= reg->threshold_len) {
	do {
	  sch_start = s + reg->dmax;
	  if (sch_start > end) sch_start = (UChar* )end;
	  if (backward_search_range(reg, str, end, sch_start, range, adjrange,
				    &low, &high) <= 0)
	    goto mismatch;

	  if (s > high)
	    s = high;

	  while (s >= low) {
	    prev = onigenc_get_prev_char_head(reg->enc, str, s, end);
	    MATCH_AND_RETURN_CHECK(orig_start);
	    s = prev;
	  }
	} while (s >= range);
	goto mismatch;
      }
      else { /* check only. */
	if ((end - range) < reg->threshold_len) goto mismatch;

	sch_start = s;
	if (reg->dmax != 0) {
	  if (reg->dmax == ONIG_INFINITE_DISTANCE)
	    sch_start = (UChar* )end;
	  else {
	    sch_start += reg->dmax;
	    if (sch_start > end) sch_start = (UChar* )end;
	    else
	      sch_start = ONIGENC_LEFT_ADJUST_CHAR_HEAD(reg->enc,
						    start, sch_start, end);
	  }
	}
	if (backward_search_range(reg, str, end, sch_start, range, adjrange,
				  &low, &high) <= 0) goto mismatch;
      }
    }

    do {
      prev = onigenc_get_prev_char_head(reg->enc, str, s, end);
      MATCH_AND_RETURN_CHECK(orig_start);
      s = prev;
    } while (s >= range);
  }

 mismatch:
#ifdef USE_FIND_LONGEST_SEARCH_ALL_OF_RANGE
  if (IS_FIND_LONGEST(reg->options)) {
    if (msa.best_len >= 0) {
      s = msa.best_s;
      goto match;
    }
  }
#endif
  r = ONIG_MISMATCH;

 finish:
  MATCH_ARG_FREE(msa);

  /* If result is mismatch and no FIND_NOT_EMPTY option,
     then the region is not set in match_at(). */
  if (IS_FIND_NOT_EMPTY(reg->options) && region) {
    onig_region_clear(region);
  }

#ifdef ONIG_DEBUG
  if (r != ONIG_MISMATCH)
    fprintf(stderr, "onig_search: error %"PRIdPTRDIFF"\n", r);
#endif
  return r;

 mismatch_no_msa:
  r = ONIG_MISMATCH;
 finish_no_msa:
#ifdef ONIG_DEBUG
  if (r != ONIG_MISMATCH)
    fprintf(stderr, "onig_search: error %"PRIdPTRDIFF"\n", r);
#endif
  return r;

 match:
  MATCH_ARG_FREE(msa);
  return s - str;
}