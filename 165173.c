MagickExport const char *GetOpenCLDeviceName(const MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return((const char *) NULL);
  return(device->name);
}