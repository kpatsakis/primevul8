MagickExport MagickBooleanType GetOneVirtualPixelInfo(const Image *image,
  const VirtualPixelMethod virtual_pixel_method,const ssize_t x,const ssize_t y,
  PixelInfo *pixel,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  register const Quantum
    *magick_restrict p;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  assert(id < (int) cache_info->number_threads);
  GetPixelInfo(image,pixel);
  p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,x,y,1UL,1UL,
    cache_info->nexus_info[id],exception);
  if (p == (const Quantum *) NULL)
    return(MagickFalse);
  GetPixelInfoPixel(image,p,pixel);
  return(MagickTrue);
}