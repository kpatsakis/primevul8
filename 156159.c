onig_regset_search(OnigRegSet* set, const UChar* str, const UChar* end,
                   const UChar* start, const UChar* range,
                   OnigRegSetLead lead, OnigOptionType option, int* rmatch_pos)
{
  int r;
  int i;
  OnigMatchParam* mp;
  OnigMatchParam** mps;

  mps = (OnigMatchParam** )xmalloc((sizeof(OnigMatchParam*) + sizeof(OnigMatchParam)) * set->n);
  CHECK_NULL_RETURN_MEMERR(mps);

  mp = (OnigMatchParam* )(mps + set->n);

  for (i = 0; i < set->n; i++) {
    onig_initialize_match_param(mp + i);
    mps[i] = mp + i;
  }

  r = onig_regset_search_with_param(set, str, end, start, range, lead, option, mps,
                                    rmatch_pos);
  for (i = 0; i < set->n; i++)
    onig_free_match_param_content(mp + i);

  xfree(mps);

  return r;
}