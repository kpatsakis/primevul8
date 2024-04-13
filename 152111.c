static const Quantum *GetVirtualPixelCache(const Image *image,
  const VirtualPixelMethod virtual_pixel_method,const ssize_t x,const ssize_t y,
  const size_t columns,const size_t rows,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  const Quantum
    *magick_restrict p;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  assert(id < (int) cache_info->number_threads);
  p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,x,y,columns,rows,
    cache_info->nexus_info[id],exception);
  return(p);
}