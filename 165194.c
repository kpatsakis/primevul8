MagickExport MagickBooleanType SetOpenCLEnabled(const MagickBooleanType value)
{
  MagickCLEnv
    clEnv;

  clEnv=GetCurrentOpenCLEnv();
  if (clEnv == (MagickCLEnv) NULL)
    return(MagickFalse);
  clEnv->enabled=value;
  return(clEnv->enabled);
}