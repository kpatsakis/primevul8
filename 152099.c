MagickPrivate MagickBooleanType CacheComponentGenesis(void)
{
  if (cache_semaphore == (SemaphoreInfo *) NULL)
    cache_semaphore=AcquireSemaphoreInfo();
  return(MagickTrue);
}