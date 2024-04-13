is_mbc_newline_ex(OnigEncoding enc, const UChar *p, const UChar *start,
		  const UChar *end, OnigOptionType option, int check_prev)
{
  if (IS_NEWLINE_CRLF(option)) {
    if (ONIGENC_MBC_TO_CODE(enc, p, end) == 0x0a) {
      if (check_prev) {
	const UChar *prev = onigenc_get_prev_char_head(enc, start, p, end);
	if ((prev != NULL) && ONIGENC_MBC_TO_CODE(enc, prev, end) == 0x0d)
	  return 0;
	else
	  return 1;
      }
      else
	return 1;
    }
    else {
      const UChar *pnext = p + enclen(enc, p, end);
      if (pnext < end &&
	  ONIGENC_MBC_TO_CODE(enc, p, end) == 0x0d &&
	  ONIGENC_MBC_TO_CODE(enc, pnext, end) == 0x0a)
	return 1;
      if (ONIGENC_IS_MBC_NEWLINE(enc, p, end))
	return 1;
      return 0;
    }
  }
  else {
    return ONIGENC_IS_MBC_NEWLINE(enc, p, end);
  }
}