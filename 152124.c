MagickExport const Quantum *GetVirtualPixelQueue(const Image *image)
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
  if (cache_info->methods.get_virtual_pixels_handler !=
       (GetVirtualPixelsHandler) NULL)
    return(cache_info->methods.get_virtual_pixels_handler(image));
  assert(id < (int) cache_info->number_threads);
  return(GetVirtualPixelsNexus(cache_info,cache_info->nexus_info[id]));
}