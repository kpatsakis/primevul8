bm_search_ic(regex_t* reg, const UChar* target, const UChar* target_end,
	     const UChar* text, const UChar* text_end, const UChar* text_range)
{
  const UChar *s, *p, *end;
  const UChar *tail;
  OnigEncoding enc = reg->enc;
  int case_fold_flag = reg->case_fold_flag;

# ifdef ONIG_DEBUG_SEARCH
  fprintf(stderr, "bm_search_ic: text: %d (%p), text_end: %d (%p), text_range: %d (%p)\n",
	  (int )text, text, (int )text_end, text_end, (int )text_range, text_range);
# endif

  end = text_range + (target_end - target) - 1;
  if (end > text_end)
    end = text_end;

  tail = target_end - 1;
  s = text + (target_end - target) - 1;
  if (IS_NULL(reg->int_map)) {
    while (s < end) {
      p = s - (target_end - target) + 1;
      if (str_lower_case_match(enc, case_fold_flag, target, target_end,
			       p, s + 1))
	return (UChar* )p;
      s += reg->map[*s];
    }
  }
  else { /* see int_map[] */
# if OPT_EXACT_MAXLEN >= ONIG_CHAR_TABLE_SIZE
    while (s < end) {
      p = s - (target_end - target) + 1;
      if (str_lower_case_match(enc, case_fold_flag, target, target_end,
			       p, s + 1))
	return (UChar* )p;
      s += reg->int_map[*s];
    }
# endif
  }
  return (UChar* )NULL;
}