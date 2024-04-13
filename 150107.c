onigenc_mbn_mbc_case_fold(OnigEncoding enc, OnigCaseFoldType flag ARG_UNUSED,
                          const UChar** pp, const UChar* end ARG_UNUSED,
			  UChar* lower)
{
  int len;
  const UChar *p = *pp;

  if (ONIGENC_IS_MBC_ASCII(p)) {
    *lower = ONIGENC_ASCII_CODE_TO_LOWER_CASE(*p);
    (*pp)++;
    return 1;
  }
  else {
    int i;

    len = enclen(enc, p, end);
    for (i = 0; i < len; i++) {
      *lower++ = *p++;
    }
    (*pp) += len;
    return len; /* return byte length of converted to lower char */
  }
}