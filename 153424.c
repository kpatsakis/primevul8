scan_number(UChar** src, const UChar* end, OnigEncoding enc)
{
  int num, val;
  OnigCodePoint c;
  UChar* p = *src;
  PFETCH_READY;

  num = 0;
  while (! PEND) {
    PFETCH(c);
    if (IS_CODE_DIGIT_ASCII(enc, c)) {
      val = (int )DIGITVAL(c);
      if ((INT_MAX - val) / 10 < num)
        return -1;  /* overflow */

      num = num * 10 + val;
    }
    else {
      PUNFETCH;
      break;
    }
  }
  *src = p;
  return num;
}