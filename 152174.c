MagickExport MagickBooleanType GetOneVirtualPixel(const Image *image,
  const ssize_t x,const ssize_t y,Quantum *pixel,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  const Quantum
    *p;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  (void) memset(pixel,0,MaxPixelChannels*sizeof(*pixel));
  if (cache_info->methods.get_one_virtual_pixel_from_handler !=
       (GetOneVirtualPixelFromHandler) NULL)
    return(cache_info->methods.get_one_virtual_pixel_from_handler(image,
      GetPixelCacheVirtualMethod(image),x,y,pixel,exception));
  assert(id < (int) cache_info->number_threads);
  p=GetVirtualPixelsFromNexus(image,GetPixelCacheVirtualMethod(image),x,y,
    1UL,1UL,cache_info->nexus_info[id],exception);
  return(CopyPixel(image,p,pixel));
}