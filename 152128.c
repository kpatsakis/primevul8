MagickPrivate void SyncAuthenticOpenCLBuffer(const Image *image)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (const Image *) NULL);
  cache_info=(CacheInfo *) image->cache;
  CopyOpenCLBuffer(cache_info);
}