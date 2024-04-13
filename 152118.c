MagickExport Quantum *GetAuthenticPixelQueue(const Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  const int
    id = GetOpenMPThreadId();

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->methods.get_authentic_pixels_from_handler !=
       (GetAuthenticPixelsFromHandler) NULL)
    return(cache_info->methods.get_authentic_pixels_from_handler(image));
  assert(id < (int) cache_info->number_threads);
  return(cache_info->nexus_info[id]->pixels);
}