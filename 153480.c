onig_strcpy(UChar* dest, const UChar* src, const UChar* end)
{
  int len = (int )(end - src);
  if (len > 0) {
    xmemcpy(dest, src, len);
    dest[len] = (UChar )0;
  }
}