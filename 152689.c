static MagickBooleanType DecodeImage(const unsigned char *compressed_pixels,
  const size_t length,unsigned char *pixels,size_t maxpixels)
{
  register const unsigned char
    *l,
    *p;

  register unsigned char
    *q;

  ssize_t
    count;

  unsigned char
    byte;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(compressed_pixels != (unsigned char *) NULL);
  assert(pixels != (unsigned char *) NULL);
  p=compressed_pixels;
  q=pixels;
  l=q+maxpixels;
  while (((size_t) (p-compressed_pixels) < length) && (q < l))
  {
    byte=(*p++);
    if (byte != 128U)
      *q++=byte;
    else
      {
        /*
          Runlength-encoded packet: <count><byte>
        */
        count=(ssize_t) (*p++);
        if (count > 0)
          byte=(*p++);
        while ((count >= 0) && (q < l))
        {
          *q++=byte;
          count--;
        }
     }
  }
  return(MagickTrue);
}