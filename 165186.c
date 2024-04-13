MagickExport MagickBooleanType GetOpenCLDeviceEnabled(
  const MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return(MagickFalse);
  return(device->enabled);
}