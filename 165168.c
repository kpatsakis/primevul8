MagickPrivate MagickCLEnv GetCurrentOpenCLEnv(void)
{
  if (default_CLEnv != (MagickCLEnv) NULL)
  {
    if ((default_CLEnv->benchmark_thread_id != (MagickThreadType) 0) &&
        (default_CLEnv->benchmark_thread_id != GetMagickThreadId()))
      return((MagickCLEnv) NULL);
    else
      return(default_CLEnv);
  }

  if (GetOpenCLCacheDirectory() == (char *) NULL)
    return((MagickCLEnv) NULL);

  if (openCL_lock == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&openCL_lock);

  LockSemaphoreInfo(openCL_lock);
  if (default_CLEnv == (MagickCLEnv) NULL)
    default_CLEnv=AcquireMagickCLEnv();
  UnlockSemaphoreInfo(openCL_lock);

  return(default_CLEnv);
}