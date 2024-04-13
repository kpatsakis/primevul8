MagickExport MagickCLDeviceType GetOpenCLDeviceType(
  const MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return(UndefinedCLDeviceType);
  if (device->type == CL_DEVICE_TYPE_GPU)
    return(GpuCLDeviceType);
  if (device->type == CL_DEVICE_TYPE_CPU)
    return(CpuCLDeviceType);
  return(UndefinedCLDeviceType);
}