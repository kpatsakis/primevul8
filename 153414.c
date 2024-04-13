callout_name_table_cmp(st_callout_name_key* x, st_callout_name_key* y)
{
  UChar *p, *q;
  int c;

  if (x->enc  != y->enc)  return 1;
  if (x->type != y->type) return 1;
  if ((x->end - x->s) != (y->end - y->s))
    return 1;

  p = x->s;
  q = y->s;
  while (p < x->end) {
    c = (int )*p - (int )*q;
    if (c != 0) return c;

    p++; q++;
  }

  return 0;
}