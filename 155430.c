static PixelChannels **DestroyPixelThreadSet(PixelChannels **pixels)
{
  register ssize_t
    i;

  assert(pixels != (PixelChannels **) NULL);
  for (i=0; i < (ssize_t) GetMagickResourceLimit(ThreadResource); i++)
    if (pixels[i] != (PixelChannels *) NULL)
      pixels[i]=(PixelChannels *) RelinquishMagickMemory(pixels[i]);
  pixels=(PixelChannels **) RelinquishMagickMemory(pixels);
  return(pixels);
}