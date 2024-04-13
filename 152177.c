MagickPrivate void CacheComponentTerminus(void)
{
  if (cache_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&cache_semaphore);
  LockSemaphoreInfo(cache_semaphore);
  instantiate_cache=MagickFalse;
  UnlockSemaphoreInfo(cache_semaphore);
  RelinquishSemaphoreInfo(&cache_semaphore);
}