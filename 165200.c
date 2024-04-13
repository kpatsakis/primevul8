MagickExport double GetOpenCLDeviceBenchmarkScore(
  const MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return(MAGICKCORE_OPENCL_UNDEFINED_SCORE);
  return(device->score);
}