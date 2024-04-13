static inline ssize_t EdgeX(const ssize_t x,const size_t columns)
{
  if (x < 0L)
    return(0L);
  if (x >= (ssize_t) columns)
    return((ssize_t) (columns-1));
  return(x);
}