static PixelList *AcquirePixelList(const size_t width,const size_t height)
{
  PixelList
    *pixel_list;

  register ssize_t
    i;

  pixel_list=(PixelList *) AcquireMagickMemory(sizeof(*pixel_list));
  if (pixel_list == (PixelList *) NULL)
    return(pixel_list);
  (void) memset((void *) pixel_list,0,sizeof(*pixel_list));
  pixel_list->length=width*height;
  for (i=0; i < ListChannels; i++)
  {
    pixel_list->lists[i].nodes=(ListNode *) AcquireAlignedMemory(65537UL,
      sizeof(*pixel_list->lists[i].nodes));
    if (pixel_list->lists[i].nodes == (ListNode *) NULL)
      return(DestroyPixelList(pixel_list));
    (void) memset(pixel_list->lists[i].nodes,0,65537UL*
      sizeof(*pixel_list->lists[i].nodes));
  }
  pixel_list->signature=MagickCoreSignature;
  return(pixel_list);
}