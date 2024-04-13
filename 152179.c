MagickPrivate Cache ReferencePixelCache(Cache cache)
{
  CacheInfo
    *magick_restrict cache_info;

  assert(cache != (Cache *) NULL);
  cache_info=(CacheInfo *) cache;
  assert(cache_info->signature == MagickCoreSignature);
  LockSemaphoreInfo(cache_info->semaphore);
  cache_info->reference_count++;
  UnlockSemaphoreInfo(cache_info->semaphore);
  return(cache_info);
}