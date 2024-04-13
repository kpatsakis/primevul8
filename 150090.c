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
    /* else if (code == 0xAA || code == 0xBA) ; */
    else if ((EncISO_8859_14_CtypeTable[code] & BIT_CTYPE_UPPER)
	     && (flags & (ONIGENC_CASE_DOWNCASE | ONIGENC_CASE_FOLD))) {
      flags |= ONIGENC_CASE_MODIFIED;
      code = ENC_ISO_8859_14_TO_LOWER_CASE(code);
    }
    else if ((EncISO_8859_14_CtypeTable[code] & BIT_CTYPE_LOWER)
	     && (flags & ONIGENC_CASE_UPCASE)) {
      flags |= ONIGENC_CASE_MODIFIED;
      if (code == 0xA2 || code == 0xA5 || code == 0xB1 || code == 0xB3 || code == 0xB5 || code == 0xBE)
	code -= 0x1;
      else if (code == 0xAB)
	code -= 0x5;
      else if (code == 0xFF)
	code -= 0x50;
      else if (code == 0xB9)
	code -= 0x2;
      else if (code == 0xBF)
	code -= 0x4;
      else if (code == 0xB8 || code == 0xBA || code == 0xBC)
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