onig_search_with_param(regex_t* reg, const UChar* str, const UChar* end,
                       const UChar* start, const UChar* range, OnigRegion* region,
                       OnigOptionType option, OnigMatchParam* mp)
{
  const UChar* data_range;

  if (range > start)
    data_range = range;
  else
    data_range = end;

  return search_in_range(reg, str, end, start, range, data_range, region,
                         option, mp);
}