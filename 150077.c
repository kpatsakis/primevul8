case_map(OnigCaseFoldType* flagP, const OnigUChar** pp,
	 const OnigUChar* end, OnigUChar* to, OnigUChar* to_end,
	 const struct OnigEncodingTypeST* enc)
{
  OnigCodePoint code;
  OnigUChar *to_start = to;
  OnigCaseFoldType flags = *flagP;

  while (*pp < end && to < to_end) {
    code = *(*pp)++;
    if (code == SHARP_s) {
      if (flags & ONIGENC_CASE_UPCASE) {
	flags |= ONIGENC_CASE_MODIFIED;
	*to++ = 'S';
	code = (flags & ONIGENC_CASE_TITLECASE) ? 's' : 'S';
      }
      else if (flags & ONIGENC_CASE_FOLD) {
	flags |= ONIGENC_CASE_MODIFIED;
	*to++ = 's';
	code = 's';
      }
    }
    else if (code == 0xB5)
      ;
    else if ((EncCP1252_CtypeTable[code] & BIT_CTYPE_UPPER)
	     && (flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD))) {
      flags |= ONIGENC_CASE_MODIFIED;
      if (code == 'I')
	code = flags & ONIGENC_CASE_FOLD_TURKISH_AZERI ? DOTLESS_i : 'i';
      else
	code = ENC_CP1252_TO_LOWER_CASE(code);
    }
    else if ((EncCP1252_CtypeTable[code]&BIT_CTYPE_LOWER)
	     && (flags & ONIGENC_CASE_UPCASE)) {
      flags |= ONIGENC_CASE_MODIFIED;
      if (code == 'i')
	code = flags & ONIGENC_CASE_FOLD_TURKISH_AZERI ? I_WITH_DOT_ABOVE : 'I';
      else if (code == DOTLESS_i)
	code = 'I';
      else if (code >= 0xB0 && code <= 0xBF)
	code -= 0x10;
      else
	code -= 0x20;
    }
    *to++ = code;
    if (flags & ONIGENC_CASE_TITLECASE)  /* switch from titlecase to lowercase for capitalize */
      flags ^= (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_TITLECASE);
  }
  *flagP = flags;
  return (int )(to - to_start);
}