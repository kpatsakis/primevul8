static MagickPixelPacket **DestroyPixelThreadSet(const Image *images,
  MagickPixelPacket **pixels)
{
  register ssize_t
    i;

  size_t
    rows;

  assert(pixels != (MagickPixelPacket **) NULL);
  rows=MagickMax(GetImageListLength(images),
    (size_t) GetMagickResourceLimit(ThreadResource));
  for (i=0; i < (ssize_t) rows; i++)
    if (pixels[i] != (MagickPixelPacket *) NULL)
      pixels[i]=(MagickPixelPacket *) RelinquishMagickMemory(pixels[i]);
  pixels=(MagickPixelPacket **) RelinquishMagickMemory(pixels);
  return(pixels);
}