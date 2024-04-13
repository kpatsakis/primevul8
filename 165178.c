MagickPrivate MagickBooleanType InitializeOpenCL(MagickCLEnv clEnv,
  ExceptionInfo *exception)
{
  LockSemaphoreInfo(clEnv->lock);
  if (clEnv->initialized != MagickFalse)
    {
      UnlockSemaphoreInfo(clEnv->lock);
      return(HasOpenCLDevices(clEnv,exception));
    }
  if (LoadOpenCLLibrary() != MagickFalse)
    {
      clEnv->library=openCL_library;
      LoadOpenCLDevices(clEnv);
      if (clEnv->number_devices > 0)
        AutoSelectOpenCLDevices(clEnv);
    }
  clEnv->initialized=MagickTrue;
  UnlockSemaphoreInfo(clEnv->lock);
  return(HasOpenCLDevices(clEnv,exception));
}