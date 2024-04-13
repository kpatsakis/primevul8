str_end_hash(st_data_t xp)
{
  const st_str_end_key *x = (const st_str_end_key *)xp;
  const UChar *p;
  st_index_t val = 0;

  p = x->s;
  while (p < x->end) {
    val = val * 997 + (int )*p++;
  }

  return val + (val >> 5);
}