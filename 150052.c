onigenc_unicode_ctype_code_range(int ctype, const OnigCodePoint* ranges[])
{
  if (ctype >= CODE_RANGES_NUM) {
    return ONIGERR_TYPE_BUG;
  }

  *ranges = CodeRanges[ctype];

  return 0;
}