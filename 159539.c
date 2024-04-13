static void GetMaximumPixelList(PixelList *pixel_list,MagickPixelPacket *pixel)
{
  register SkipList
    *list;

  register ssize_t
    channel;

  size_t
    color,
    maximum;

  ssize_t
    count;

  unsigned short
    channels[ListChannels];

  /*
    Find the maximum value for each of the color.
  */
  for (channel=0; channel < 5; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    count=0;
    maximum=list->nodes[color].next[0];
    do
    {
      color=list->nodes[color].next[0];
      if (color > maximum)
        maximum=color;
      count+=list->nodes[color].count;
    } while (count < (ssize_t) pixel_list->length);
    channels[channel]=(unsigned short) maximum;
  }
  pixel->red=(MagickRealType) ScaleShortToQuantum(channels[0]);
  pixel->green=(MagickRealType) ScaleShortToQuantum(channels[1]);
  pixel->blue=(MagickRealType) ScaleShortToQuantum(channels[2]);
  pixel->opacity=(MagickRealType) ScaleShortToQuantum(channels[3]);
  pixel->index=(MagickRealType) ScaleShortToQuantum(channels[4]);
}