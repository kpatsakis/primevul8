static PixelList *DestroyPixelList(PixelList *pixel_list)
{
  register ssize_t
    i;

  if (pixel_list == (PixelList *) NULL)
    return((PixelList *) NULL);
  for (i=0; i < ListChannels; i++)
    if (pixel_list->lists[i].nodes != (ListNode *) NULL)
      pixel_list->lists[i].nodes=(ListNode *) RelinquishAlignedMemory(
        pixel_list->lists[i].nodes);
  pixel_list=(PixelList *) RelinquishMagickMemory(pixel_list);
  return(pixel_list);
}