onig_match(regex_t* reg, const UChar* str, const UChar* end, const UChar* at,
           OnigRegion* region, OnigOptionType option)
{
  int r;
  OnigMatchParam mp;

  onig_initialize_match_param(&mp);
  r = onig_match_with_param(reg, str, end, at, region, option, &mp);
  onig_free_match_param_content(&mp);
  return r;
}