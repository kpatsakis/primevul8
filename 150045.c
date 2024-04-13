onigenc_unicode_is_code_ctype(OnigCodePoint code, unsigned int ctype, OnigEncoding enc ARG_UNUSED)
{
  if (
#ifdef USE_UNICODE_PROPERTIES
      ctype <= ONIGENC_MAX_STD_CTYPE &&
#endif
      code < 256) {
    return ONIGENC_IS_UNICODE_ISO_8859_1_CTYPE(code, ctype);
  }

  if (ctype >= CODE_RANGES_NUM) {
    return ONIGERR_TYPE_BUG;
  }

  return onig_is_in_code_range((UChar* )CodeRanges[ctype], code);
}