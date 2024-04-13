MagickExport void SetOpenCLDeviceEnabled(MagickCLDevice device,
  const MagickBooleanType value)
{
  if (device == (MagickCLDevice) NULL)
    return;
  device->enabled=value;
}