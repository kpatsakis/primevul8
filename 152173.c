MagickExport Quantum *GetAuthenticPixels(Image *image,const ssize_t x,
  const ssize_t y,const size_t columns,const size_t rows,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  Quantum
    *pixels;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->methods.get_authentic_pixels_handler !=
      (GetAuthenticPixelsHandler) NULL)
    {
      pixels=cache_info->methods.get_authentic_pixels_handler(image,x,y,columns,
        rows,exception);
      return(pixels);
    }
  assert(id < (int) cache_info->number_threads);
  pixels=GetAuthenticPixelCacheNexus(image,x,y,columns,rows,
    cache_info->nexus_info[id],exception);
  return(pixels);
}