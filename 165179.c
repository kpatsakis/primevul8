MagickExport MagickBooleanType GetOpenCLEnabled(void)
{
  MagickCLEnv
    clEnv;

  clEnv=GetCurrentOpenCLEnv();
  if (clEnv == (MagickCLEnv) NULL)
    return(MagickFalse);
  return(clEnv->enabled);
}