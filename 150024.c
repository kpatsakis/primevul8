add_code_range(BBuf** pbuf, ScanEnv* env, OnigCodePoint from, OnigCodePoint to)
{
  return add_code_range0(pbuf, env, from, to, 1);
}