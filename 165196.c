static MagickCLEnv AcquireMagickCLEnv(void)
{
  const char
    *option;

  MagickCLEnv
    clEnv;

  clEnv=(MagickCLEnv) AcquireMagickMemory(sizeof(*clEnv));
  if (clEnv != (MagickCLEnv) NULL)
  {
    (void) ResetMagickMemory(clEnv,0,sizeof(*clEnv));
    ActivateSemaphoreInfo(&clEnv->lock);
    clEnv->cpu_score=MAGICKCORE_OPENCL_UNDEFINED_SCORE;
    clEnv->enabled=MagickTrue;
    option=getenv("MAGICK_OCL_DEVICE");
    if ((option != (const char *) NULL) && (strcmp(option,"OFF") == 0))
      clEnv->enabled=MagickFalse;
  }
  return clEnv;
}