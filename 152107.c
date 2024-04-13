MagickPrivate VirtualPixelMethod GetPixelCacheVirtualMethod(const Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  return(cache_info->virtual_pixel_method);
}