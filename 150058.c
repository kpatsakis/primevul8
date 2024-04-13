is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp, const UChar* end)
{
  int v;
  const UChar* p = *pp;

  if (*p == SHARP_s && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    (*pp)++;
    return TRUE;
  }

  (*pp)++;
  v = (EncISO_8859_13_CtypeTable[*p] & (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
  if ((v | BIT_CTYPE_LOWER) != 0) {
    /* 0xdf, 0xb5 are lower case letter, but can't convert. */
    if (*p == 0xb5)
      return FALSE;
    else
      return TRUE;
  }

  return (v != 0 ? TRUE : FALSE);
}