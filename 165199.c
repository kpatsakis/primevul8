MagickExport const char *GetOpenCLDeviceVersion(const MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return((const char *) NULL);
  return(device->version);
}