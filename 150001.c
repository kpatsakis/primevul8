str_end_cmp(st_data_t xp, st_data_t yp)
{
  const st_str_end_key *x, *y;
  const UChar *p, *q;
  int c;

  x = (const st_str_end_key *)xp;
  y = (const st_str_end_key *)yp;
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