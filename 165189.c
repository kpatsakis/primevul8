static MagickCLEnv RelinquishMagickCLEnv(MagickCLEnv clEnv)
{
  if (clEnv == (MagickCLEnv) NULL)
    return((MagickCLEnv) NULL);

  RelinquishSemaphoreInfo(&clEnv->lock);
  RelinquishMagickCLDevices(clEnv);
  if (clEnv->contexts != (cl_context *) NULL)
    {
      ssize_t
        i;

      for (i=0; i < clEnv->number_contexts; i++)
         (void) openCL_library->clReleaseContext(clEnv->contexts[i]);
      clEnv->contexts=(cl_context *) RelinquishMagickMemory(clEnv->contexts);
    }
  return((MagickCLEnv) RelinquishMagickMemory(clEnv));
}