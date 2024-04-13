parse_long(OnigEncoding enc, UChar* s, UChar* end, int sign_on, long max, long* rl)
{
  long v;
  long d;
  int flag;
  UChar* p;
  OnigCodePoint c;

  if (s >= end) return ONIGERR_INVALID_CALLOUT_ARG;

  flag = 1;
  v = 0;
  p = s;
  while (p < end) {
    c = ONIGENC_MBC_TO_CODE(enc, p, end);
    p += ONIGENC_MBC_ENC_LEN(enc, p);
    if (c >= '0' && c <= '9') {
      d = (long )(c - '0');
      if (v > (max - d) / 10)
        return ONIGERR_INVALID_CALLOUT_ARG;

      v = v * 10 + d;
    }
    else if (sign_on != 0 && (c == '-' || c == '+')) {
      if (c == '-') flag = -1;
    }
    else
      return ONIGERR_INVALID_CALLOUT_ARG;

    sign_on = 0;
  }

  *rl = flag * v;
  return ONIG_NORMAL;
}