gb18030_mbc_enc_len(const UChar* p)
{
  if (GB18030_MAP[*p] != CM)
    return 1;

  p++;
  if (GB18030_MAP[*p] == C4)
    return 4;

  return 2;
}