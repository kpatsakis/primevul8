str_end_hash(st_str_end_key* x)
{
  UChar *p;
  unsigned val = 0;

  p = x->s;
  while (p < x->end) {
    val = val * 997 + (unsigned )*p++;
  }

  return (int) (val + (val >> 5));
}