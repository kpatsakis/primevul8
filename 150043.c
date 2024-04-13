onigenc_ascii_only_case_map(OnigCaseFoldType* flagP, const OnigUChar** pp, const OnigUChar* end,
			    OnigUChar* to, OnigUChar* to_end, const struct OnigEncodingTypeST* enc)
{
  OnigCodePoint code;
  OnigUChar *to_start = to;
  OnigCaseFoldType flags = *flagP;
  int codepoint_length;

  while (*pp < end && to < to_end) {
    codepoint_length = ONIGENC_PRECISE_MBC_ENC_LEN(enc, *pp, end);
    if (codepoint_length < 0)
      return codepoint_length; /* encoding invalid */
    code = ONIGENC_MBC_TO_CODE(enc, *pp, end);
    *pp += codepoint_length;

    if (code >= 'a' && code <= 'z' && (flags & ONIGENC_CASE_UPCASE)) {
      flags |= ONIGENC_CASE_MODIFIED;
      code += 'A' - 'a';
    } else if (code >= 'A' && code <= 'Z' &&
	(flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD))) {
      flags |= ONIGENC_CASE_MODIFIED;
      code += 'a' - 'A';
    }
    to += ONIGENC_CODE_TO_MBC(enc, code, to);
    if (flags & ONIGENC_CASE_TITLECASE)  /* switch from titlecase to lowercase for capitalize */
      flags ^= (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_TITLECASE);
  }
  *flagP = flags;
  return (int )(to - to_start);
}