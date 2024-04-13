static void GetStandardDeviationPixelList(PixelList *pixel_list,
  MagickPixelPacket *pixel)
{
  MagickRealType
    sum,
    sum_squared;

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
    Find the standard-deviation value for each of the color.
  */
  for (channel=0; channel < 5; channel++)
  {
    list=pixel_list->lists+channel;
    color=65536L;
    count=0;
    sum=0.0;
    sum_squared=0.0;
    do
    {
      register ssize_t
        i;

      color=list->nodes[color].next[0];
      sum+=(MagickRealType) list->nodes[color].count*color;
      for (i=0; i < (ssize_t) list->nodes[color].count; i++)
        sum_squared+=((MagickRealType) color)*((MagickRealType) color);
      count+=list->nodes[color].count;
    } while (count < (ssize_t) pixel_list->length);
    sum/=pixel_list->length;
    sum_squared/=pixel_list->length;
    channels[channel]=(unsigned short) sqrt(sum_squared-(sum*sum));
  }
  pixel->red=(MagickRealType) ScaleShortToQuantum(channels[0]);
  pixel->green=(MagickRealType) ScaleShortToQuantum(channels[1]);
  pixel->blue=(MagickRealType) ScaleShortToQuantum(channels[2]);
  pixel->opacity=(MagickRealType) ScaleShortToQuantum(channels[3]);
  pixel->index=(MagickRealType) ScaleShortToQuantum(channels[4]);
}