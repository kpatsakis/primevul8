MagickExport void SetOpenCLKernelProfileEnabled(MagickCLDevice device,
  const MagickBooleanType value)
{
  if (device == (MagickCLDevice) NULL)
    return;
  device->profile_kernels=value;
}