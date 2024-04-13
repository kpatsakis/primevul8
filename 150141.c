find_str_position(OnigCodePoint s[], int n, UChar* from, UChar* to,
		  UChar **next, OnigEncoding enc)
{
  int i;
  OnigCodePoint x;
  UChar *q;
  UChar *p = from;

  while (p < to) {
    x = ONIGENC_MBC_TO_CODE(enc, p, to);
    q = p + enclen(enc, p, to);
    if (x == s[0]) {
      for (i = 1; i < n && q < to; i++) {
	x = ONIGENC_MBC_TO_CODE(enc, q, to);
	if (x != s[i]) break;
	q += enclen(enc, q, to);
      }
      if (i >= n) {
	if (IS_NOT_NULL(next))
	  *next = q;
	return p;
      }
    }
    p = q;
  }
  return NULL_UCHARP;
}