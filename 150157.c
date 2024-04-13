bits_at(const OnigCodePoint *c, const int n)
{
  return bits_of(c[n / 3], n % 3);
}