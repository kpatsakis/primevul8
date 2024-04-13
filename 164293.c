is_valid_mbc_string(const UChar* p, const UChar* end)
{
  while (p < end) {
    if (*p < 0x80) {
      p++;
    }
    else if (*p == 0x80 || *p == 0xff) {
      return FALSE;
    }
    else {
      p++;
      if (p >= end) return FALSE;
      if (*p < 0x40) {
        if (*p < 0x30 || *p > 0x39)
          return FALSE;

        p++;
        if (p >= end) return FALSE;
        if (*p < 0x81 || *p == 0xff) return FALSE;

        p++;
        if (p >= end) return FALSE;
        if (*p < 0x30 || *p > 0x39)
          return FALSE;

        p++;
      }
      else if (*p == 0x7f || *p == 0xff) {
        return FALSE;
      }
      else {
        p++;
      }
    }
  }

  return TRUE;
}