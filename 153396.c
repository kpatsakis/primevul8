scan_octal_number(UChar** src, UChar* end, int minlen, int maxlen,
                  OnigEncoding enc, OnigCodePoint* rcode)
{
  OnigCodePoint code;
  OnigCodePoint c;
  unsigned int val;
  int n;
  UChar* p = *src;
  PFETCH_READY;

  code = 0;
  n = 0;
  while (! PEND && n < maxlen) {
    PFETCH(c);
    if (IS_CODE_DIGIT_ASCII(enc, c) && c < '8') {
      n++;
      val = (unsigned int )ODIGITVAL(c);
      if ((UINT_MAX - val) / 8UL < code)
        return ONIGERR_TOO_BIG_NUMBER; /* overflow */

      code = (code << 3) + val;
    }
    else {
      PUNFETCH;
      break;
    }
  }

  if (n < minlen)
    return ONIGERR_INVALID_CODE_POINT_VALUE;

  *rcode = code;
  *src = p;
  return ONIG_NORMAL;
}