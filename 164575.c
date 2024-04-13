static void CorrectPhaseLHS(const size_t width,const size_t height,
  double *fourier_pixels)
{
  register ssize_t
    x;

  ssize_t
    y;

  for (y=0L; y < (ssize_t) height; y++)
    for (x=0L; x < (ssize_t) (width/2L); x++)
      fourier_pixels[y*width+x]*=(-1.0);
}