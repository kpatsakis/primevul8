case_map(OnigCaseFoldType* flagP, const OnigUChar** pp,
	 const OnigUChar* end, OnigUChar* to, OnigUChar* to_end,
	 const struct OnigEncodingTypeST* enc)
{
  OnigCodePoint code;
  OnigUChar *to_start = to;
  OnigCaseFoldType flags = *flagP;

  while (*pp < end && to < to_end) {
    code = *(*pp)++;
    if (code == 0xF2) {
      if (flags & ONIGENC_CASE_UPCASE) {
	flags |= ONIGENC_CASE_MODIFIED;
	code = 0xD3;
      }
      else if (flags & ONIGENC_CASE_FOLD) {
	flags |= ONIGENC_CASE_MODIFIED;
	code = 0xF3;
      }
    }
    else if ((EncISO_8859_7_CtypeTable[code] & BIT_CTYPE_UPPER)
	     && (flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD))) {
      flags |= ONIGENC_CASE_MODIFIED;
      code = ENC_ISO_8859_7_TO_LOWER_CASE(code);
    }
    else if (code == 0xC0 || code == 0xE0)
      ;
    else if ((EncISO_8859_7_CtypeTable[code]&BIT_CTYPE_LOWER)
	     && (flags & ONIGENC_CASE_UPCASE)) {
      flags |= ONIGENC_CASE_MODIFIED;
      if (code == 0xDC) {
	code -= 0x26;
      }
      else if (code >= 0xDD && code <= 0xDF) {
	code -= 0x25;
      }
      else if (code == 0xFC) {
	code -= 0x40;
      }
      else if (code == 0xFD || code == 0xFE) {
	code -= 0x3F;
      }
      else {
	code -= 0x20;
      }
    }
    *to++ = code;
    if (flags & ONIGENC_CASE_TITLECASE)  /* switch from titlecase to lowercase for capitalize */
      flags ^= (ONIGENC_CASE_UPCASE | ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_TITLECASE);
  }
  *flagP = flags;
  return (int )(to - to_start);
}