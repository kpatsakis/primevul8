static inline ssize_t RandomY(RandomInfo *random_info,const size_t rows)
{
  return((ssize_t) (rows*GetPseudoRandomValue(random_info)));
}