static Quantum *GetAuthenticPixelsCache(Image *image,const ssize_t x,
  const ssize_t y,const size_t columns,const size_t rows,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  Quantum
    *magick_restrict pixels;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  if (cache_info == (Cache) NULL)
    return((Quantum *) NULL);
  assert(cache_info->signature == MagickCoreSignature);
  assert(id < (int) cache_info->number_threads);
  pixels=GetAuthenticPixelCacheNexus(image,x,y,columns,rows,
    cache_info->nexus_info[id],exception);
  return(pixels);
}