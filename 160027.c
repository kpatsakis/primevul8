float LibRaw::int_to_float(int i)
{
  union {
    int i;
    float f;
  } u;
  u.i = i;
  return u.f;
}