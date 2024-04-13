static void GetNonpeakPixelList(PixelList *pixel_list,MagickPixelPacket *pixel)
{
  register SkipList
    *list;

  register ssize_t
    channel;

  size_t
    color,
    next,
    previous;

  ssize_t
    count;

  unsigned short
    channels[5];

  /*
    Finds the non peak value for each of the colors.
  */
  for (channel=0; channel < 5; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    next=list->nodes[color].next[0];
    count=0;
    do
    {
      previous=color;
      color=next;
      next=list->nodes[color].next[0];
      count+=list->nodes[color].count;
    } while (count <= (ssize_t) (pixel_list->length >> 1));
    if ((previous == 65536UL) && (next != 65536UL))
      color=next;
    else
      if ((previous != 65536UL) && (next == 65536UL))
        color=previous;
    channels[channel]=(unsigned short) color;
  }
  pixel->red=(MagickRealType) ScaleShortToQuantum(channels[0]);
  pixel->green=(MagickRealType) ScaleShortToQuantum(channels[1]);
  pixel->blue=(MagickRealType) ScaleShortToQuantum(channels[2]);
  pixel->opacity=(MagickRealType) ScaleShortToQuantum(channels[3]);
  pixel->index=(MagickRealType) ScaleShortToQuantum(channels[4]);
}