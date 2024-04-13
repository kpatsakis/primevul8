MagickExport void *GetPixelCachePixels(Image *image,MagickSizeType *length,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  assert(length != (MagickSizeType *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  *length=cache_info->length;
  if ((cache_info->type != MemoryCache) && (cache_info->type != MapCache))
    return((void *) NULL);
  return((void *) cache_info->pixels);
}