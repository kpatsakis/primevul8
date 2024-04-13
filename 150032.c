is_mbc_ambiguous(OnigCaseFoldType flag, const UChar** pp, const UChar* end)
{
  int v;
  const UChar* p = *pp;

  if (*p == SHARP_s && (flag & INTERNAL_ONIGENC_CASE_FOLD_MULTI_CHAR) != 0) {
    (*pp)++;
    return TRUE;
  }

  (*pp)++;
  v = (EncISO_8859_2_CtypeTable[*p] & (BIT_CTYPE_UPPER | BIT_CTYPE_LOWER));
  if ((v | BIT_CTYPE_LOWER) != 0) {
    return TRUE;
  }

  return (v != 0 ? TRUE : FALSE);
}