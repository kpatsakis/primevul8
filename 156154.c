regset_search_body_regex_lead(OnigRegSet* set,
              const UChar* str, const UChar* end,
              const UChar* start, const UChar* orig_range, OnigRegSetLead lead,
              OnigOptionType option, OnigMatchParam* mps[], int* rmatch_pos)
{
  int r;
  int i;
  int n;
  int match_index;
  const UChar* ep;
  regex_t* reg;
  OnigRegion* region;

  n = set->n;

  match_index = ONIG_MISMATCH;
  ep = orig_range;
  for (i = 0; i < n; i++) {
    reg    = set->rs[i].reg;
    region = set->rs[i].region;
    r = search_in_range(reg, str, end, start, ep, orig_range, region, option, mps[i]);
    if (r > 0) {
      if (str + r < ep) {
        match_index = i;
        *rmatch_pos = r;
        if (lead == ONIG_REGSET_PRIORITY_TO_REGEX_ORDER)
          break;

        ep = str + r;
      }
    }
    else if (r == 0) {
      match_index = i;
      *rmatch_pos = r;
      break;
    }
  }

  return match_index;
}