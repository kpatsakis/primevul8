static void CacheOpenCLBenchmarks(MagickCLEnv clEnv)
{
  char
    filename[MagickPathExtent];

  FILE
    *cache_file;

  MagickCLDevice
    device;

  size_t
    i,
    j;

  (void) FormatLocaleString(filename,MagickPathExtent,"%s%s%s",
    GetOpenCLCacheDirectory(),DirectorySeparator,
    IMAGEMAGICK_PROFILE_FILE);

  cache_file=fopen_utf8(filename,"wb");
  if (cache_file == (FILE *) NULL)
    return;
  fwrite("<devices>\n",sizeof(char),10,cache_file);
  fprintf(cache_file,"  <device name=\"CPU\" score=\"%.4g\"/>\n",
    clEnv->cpu_score);
  for (i = 0; i < clEnv->number_devices; i++)
  {
    MagickBooleanType
      duplicate;

    device=clEnv->devices[i];
    duplicate=MagickFalse;
    for (j = 0; j < i; j++)
    {
      if (IsSameOpenCLDevice(clEnv->devices[j],device))
      {
        duplicate=MagickTrue;
        break;
      }
    }

    if (duplicate)
      continue;

    if (device->score != MAGICKCORE_OPENCL_UNDEFINED_SCORE)
      fprintf(cache_file,"  <device platform=\"%s\" vendor=\"%s\" name=\"%s\"\
 version=\"%s\" maxClockFrequency=\"%d\" maxComputeUnits=\"%d\"\
 score=\"%.4g\"/>\n",
        device->platform_name,device->vendor_name,device->name,device->version,
        (int)device->max_clock_frequency,(int)device->max_compute_units,
        device->score);
  }
  fwrite("</devices>",sizeof(char),10,cache_file);

  fclose(cache_file);
}