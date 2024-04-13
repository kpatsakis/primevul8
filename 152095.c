static inline ssize_t DitherY(const ssize_t y,const size_t rows)
{
  ssize_t
    index;

  index=y+DitherMatrix[y & 0x07]-32L;
  if (index < 0L)
    return(0L);
  if (index >= (ssize_t) rows)
    return((ssize_t) rows-1L);
  return(index);
}