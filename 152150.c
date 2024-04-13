static inline MagickModulo VirtualPixelModulo(const ssize_t offset,
  const size_t extent)
{
  MagickModulo
    modulo;

  /*
    Compute the remainder of dividing offset by extent.  It returns not only
    the quotient (tile the offset falls in) but also the positive remainer
    within that tile such that 0 <= remainder < extent.  This method is
    essentially a ldiv() using a floored modulo division rather than the
    normal default truncated modulo division.
  */
  modulo.quotient=offset/(ssize_t) extent;
  if (offset < 0L)
    modulo.quotient--;
  modulo.remainder=offset-modulo.quotient*(ssize_t) extent;
  return(modulo);
}