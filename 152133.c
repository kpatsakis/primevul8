MagickPrivate const void *AcquirePixelCachePixels(const Image *image,
  MagickSizeType *length,ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  *length=0;
  if ((cache_info->type != MemoryCache) && (cache_info->type != MapCache))
    return((const void *) NULL);
  *length=cache_info->length;
  return((const void *) cache_info->pixels);
}