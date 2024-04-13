and_code_range1(BBuf** pbuf, ScanEnv* env, OnigCodePoint from1, OnigCodePoint to1,
		OnigCodePoint* data, int n)
{
  int i, r;
  OnigCodePoint from2, to2;

  for (i = 0; i < n; i++) {
    from2 = data[i*2];
    to2   = data[i*2+1];
    if (from2 < from1) {
      if (to2 < from1) continue;
      else {
	from1 = to2 + 1;
      }
    }
    else if (from2 <= to1) {
      if (to2 < to1) {
	if (from1 <= from2 - 1) {
	  r = add_code_range_to_buf(pbuf, env, from1, from2-1);
	  if (r != 0) return r;
	}
	from1 = to2 + 1;
      }
      else {
	to1 = from2 - 1;
      }
    }
    else {
      from1 = from2;
    }
    if (from1 > to1) break;
  }
  if (from1 <= to1) {
    r = add_code_range_to_buf(pbuf, env, from1, to1);
    if (r != 0) return r;
  }
  return 0;
}