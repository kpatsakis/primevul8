static void GetMedianPixelList(PixelList *pixel_list,MagickPixelPacket *pixel)
{
  register SkipList
    *list;

  register ssize_t
    channel;

  size_t
    color;

  ssize_t
    count;

  unsigned short
    channels[ListChannels];

  /*
    Find the median value for each of the color.
  */
  for (channel=0; channel < 5; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    count=0;
    do
    {
      color=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    } while (count <= (ssize_t) (pixel_list->length >> 1));
    channels[channel]=(unsigned short) color;
  }
  GetMagickPixelPacket((const Image *) NULL,pixel);
  pixel->red=(MagickRealType) ScaleShortToQuantum(channels[0]);
  pixel->green=(MagickRealType) ScaleShortToQuantum(channels[1]);
  pixel->blue=(MagickRealType) ScaleShortToQuantum(channels[2]);
  pixel->opacity=(MagickRealType) ScaleShortToQuantum(channels[3]);
  pixel->index=(MagickRealType) ScaleShortToQuantum(channels[4]);
}