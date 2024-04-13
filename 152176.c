MagickPrivate ClassType GetPixelCacheStorageClass(const Cache cache)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(cache != (Cache) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      cache_info->filename);
  return(cache_info->storage_class);
}