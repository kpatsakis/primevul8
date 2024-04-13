add_code_range_to_buf(BBuf** pbuf, ScanEnv* env, OnigCodePoint from, OnigCodePoint to)
{
  return add_code_range_to_buf0(pbuf, env, from, to, 1);
}