onig_match_with_param(regex_t* reg, const UChar* str, const UChar* end,
                      const UChar* at, OnigRegion* region, OnigOptionType option,
                      OnigMatchParam* mp)
{
  int r;
  UChar *prev;
  MatchArg msa;

  ADJUST_MATCH_PARAM(reg, mp);
  MATCH_ARG_INIT(msa, reg, option, region, at, mp);
  if (region
#ifdef USE_POSIX_API_REGION_OPTION
      && !IS_POSIX_REGION(option)
#endif
      ) {
    r = onig_region_resize_clear(region, reg->num_mem + 1);
  }
  else
    r = 0;

  if (r == 0) {
    if (ONIG_IS_OPTION_ON(option, ONIG_OPTION_CHECK_VALIDITY_OF_STRING)) {
      if (! ONIGENC_IS_VALID_MBC_STRING(reg->enc, str, end)) {
        r = ONIGERR_INVALID_WIDE_CHAR_VALUE;
        goto end;
      }
    }

    prev = (UChar* )onigenc_get_prev_char_head(reg->enc, str, at);
    r = match_at(reg, str, end, end, at, prev, &msa);
  }

 end:
  MATCH_ARG_FREE(msa);
  return r;
}