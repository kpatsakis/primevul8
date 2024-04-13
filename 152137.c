static MagickBooleanType GetOneAuthenticPixelFromCache(Image *image,
  const ssize_t x,const ssize_t y,Quantum *pixel,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  register Quantum
    *magick_restrict q;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  assert(id < (int) cache_info->number_threads);
  (void) memset(pixel,0,MaxPixelChannels*sizeof(*pixel));
  q=GetAuthenticPixelCacheNexus(image,x,y,1UL,1UL,cache_info->nexus_info[id],
    exception);
  return(CopyPixel(image,q,pixel));
}