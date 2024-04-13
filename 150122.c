distance_multiply(OnigDistance d, int m)
{
  if (m == 0) return 0;

  if (d < ONIG_INFINITE_DISTANCE / m)
    return d * m;
  else
    return ONIG_INFINITE_DISTANCE;
}