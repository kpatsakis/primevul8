static inline MagickOffsetType ReadPixelCacheRegion(
  const CacheInfo *magick_restrict cache_info,const MagickOffsetType offset,
  const MagickSizeType length,unsigned char *magick_restrict buffer)
{
  register MagickOffsetType
    i;

  ssize_t
    count;

#if !defined(MAGICKCORE_HAVE_PREAD)
  if (lseek(cache_info->file,offset,SEEK_SET) < 0)
    return((MagickOffsetType) -1);
#endif
  count=0;
  for (i=0; i < (MagickOffsetType) length; i+=count)
  {
#if !defined(MAGICKCORE_HAVE_PREAD)
    count=read(cache_info->file,buffer+i,(size_t) MagickMin(length-i,(size_t)
      SSIZE_MAX));
#else
    count=pread(cache_info->file,buffer+i,(size_t) MagickMin(length-i,(size_t)
      SSIZE_MAX),(off_t) (offset+i));
#endif
    if (count <= 0)
      {
        count=0;
        if (errno != EINTR)
          break;
      }
  }
  return(i);
}