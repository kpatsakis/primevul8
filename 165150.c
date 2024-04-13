MagickPrivate void OpenCLTerminus()
{
  DumpOpenCLProfileData();
  if (cache_directory != (char *) NULL)
    cache_directory=DestroyString(cache_directory);
  if (cache_directory_lock != (SemaphoreInfo *) NULL)
    RelinquishSemaphoreInfo(&cache_directory_lock);
  if (default_CLEnv != (MagickCLEnv) NULL)
    default_CLEnv=RelinquishMagickCLEnv(default_CLEnv);
  if (openCL_lock != (SemaphoreInfo *) NULL)
    RelinquishSemaphoreInfo(&openCL_lock);
  if (openCL_library != (MagickLibrary *) NULL)
    {
      if (openCL_library->library != (void *) NULL)
        (void) lt_dlclose(openCL_library->library);
      openCL_library=(MagickLibrary *) RelinquishMagickMemory(openCL_library);
    }
}