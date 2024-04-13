MagickPrivate Quantum *GetAuthenticPixelCacheNexus(Image *image,const ssize_t x,
  const ssize_t y,const size_t columns,const size_t rows,NexusInfo *nexus_info,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  Quantum
    *magick_restrict pixels;

  /*
    Transfer pixels from the cache.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  pixels=QueueAuthenticPixelCacheNexus(image,x,y,columns,rows,MagickTrue,
    nexus_info,exception);
  if (pixels == (Quantum *) NULL)
    return((Quantum *) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (nexus_info->authentic_pixel_cache != MagickFalse)
    return(pixels);
  if (ReadPixelCachePixels(cache_info,nexus_info,exception) == MagickFalse)
    return((Quantum *) NULL);
  if (cache_info->metacontent_extent != 0)
    if (ReadPixelCacheMetacontent(cache_info,nexus_info,exception) == MagickFalse)
      return((Quantum *) NULL);
  return(pixels);
}