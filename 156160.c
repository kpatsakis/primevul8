onig_search(regex_t* reg, const UChar* str, const UChar* end,
            const UChar* start, const UChar* range, OnigRegion* region,
            OnigOptionType option)
{
  int r;
  OnigMatchParam mp;
  const UChar* data_range;

  onig_initialize_match_param(&mp);

  /* The following is an expanded code of onig_search_with_param()  */
  if (range > start)
    data_range = range;
  else
    data_range = end;

  r = search_in_range(reg, str, end, start, range, data_range, region,
                      option, &mp);

  onig_free_match_param_content(&mp);
  return r;

}