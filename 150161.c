onigenc_is_mbc_newline_0x0a(const UChar* p, const UChar* end, OnigEncoding enc ARG_UNUSED)
{
  if (p < end) {
    if (*p == 0x0a) return 1;
  }
  return 0;
}