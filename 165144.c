MagickExport MagickCLDevice *GetOpenCLDevices(size_t *length,
  ExceptionInfo *exception)
{
  MagickCLEnv
    clEnv;

  clEnv=GetCurrentOpenCLEnv();
  if (clEnv == (MagickCLEnv) NULL)
    {
      if (length != (size_t *) NULL)
        *length=0;
      return((MagickCLDevice *) NULL);
    }
  InitializeOpenCL(clEnv,exception);
  if (length != (size_t *) NULL)
    *length=clEnv->number_devices;
  return(clEnv->devices);
}