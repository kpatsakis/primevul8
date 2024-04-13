static void GetModePixelList(PixelList *pixel_list,MagickPixelPacket *pixel)
{
  register SkipList
    *list;

  register ssize_t
    channel;

  size_t
    color,
    max_count,
    mode;

  ssize_t
    count;

  unsigned short
    channels[5];

  /*
    Make each pixel the 'predominant color' of the specified neighborhood.
  */
  for (channel=0; channel < 5; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    mode=color;
    max_count=list->nodes[mode].count;
    count=0;
    do
    {
      color=list->nodes[color].next[0];
      if (list->nodes[color].count > max_count)
        {
          mode=color;
          max_count=list->nodes[mode].count;
        }
      count+=list->nodes[color].count;
    } while (count < (ssize_t) pixel_list->length);
    channels[channel]=(unsigned short) mode;
  }
  pixel->red=(MagickRealType) ScaleShortToQuantum(channels[0]);
  pixel->green=(MagickRealType) ScaleShortToQuantum(channels[1]);
  pixel->blue=(MagickRealType) ScaleShortToQuantum(channels[2]);
  pixel->opacity=(MagickRealType) ScaleShortToQuantum(channels[3]);
  pixel->index=(MagickRealType) ScaleShortToQuantum(channels[4]);
}