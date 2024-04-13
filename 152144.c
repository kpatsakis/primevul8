MagickPrivate void ResetPixelCacheChannels(Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  cache_info->number_channels=GetPixelChannels(image);
}