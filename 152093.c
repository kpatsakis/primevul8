static inline MagickBooleanType CopyPixel(const Image *image,
  const Quantum *source,Quantum *destination)
{
  register ssize_t
    i;

  if (source == (const Quantum *) NULL)
    {
      destination[RedPixelChannel]=ClampToQuantum(image->background_color.red);
      destination[GreenPixelChannel]=ClampToQuantum(
        image->background_color.green);
      destination[BluePixelChannel]=ClampToQuantum(
        image->background_color.blue);
      destination[BlackPixelChannel]=ClampToQuantum(
        image->background_color.black);
      destination[AlphaPixelChannel]=ClampToQuantum(
        image->background_color.alpha);
      return(MagickFalse);
    }
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    PixelChannel channel = GetPixelChannelChannel(image,i);
    destination[channel]=source[i];
  }
  return(MagickTrue);
}