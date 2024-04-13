static MagickBooleanType LoadOpenCLBenchmarks(MagickCLEnv clEnv)
{
  char
    filename[MagickPathExtent];

  StringInfo
    *option;

  size_t
    i;

  (void) FormatLocaleString(filename,MagickPathExtent,"%s%s%s",
    GetOpenCLCacheDirectory(),DirectorySeparator,IMAGEMAGICK_PROFILE_FILE);

  /*
    We don't run the benchmark when we can not write out a device profile. The
    first GPU device will be used.
  */
#if !defined(MAGICKCORE_ZERO_CONFIGURATION_SUPPORT)
  if (CanWriteProfileToFile(filename) == MagickFalse)
#endif
    {
      for (i = 0; i < clEnv->number_devices; i++)
        clEnv->devices[i]->score=1.0;

      SelectOpenCLDevice(clEnv,CL_DEVICE_TYPE_GPU);
      return(MagickFalse);
    }

  option=ConfigureFileToStringInfo(filename);
  LoadOpenCLDeviceBenchmark(clEnv,(const char *) GetStringInfoDatum(option));
  option=DestroyStringInfo(option);
  return(MagickTrue);
}