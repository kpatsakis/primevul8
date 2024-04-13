static void AutoSelectOpenCLDevices(MagickCLEnv clEnv)
{
  const char
    *option;

  double
    best_score;

  MagickBooleanType
    benchmark;

  size_t
    i;

  option=getenv("MAGICK_OCL_DEVICE");
  if (option != (const char *) NULL)
    {
      if (strcmp(option,"GPU") == 0)
        SelectOpenCLDevice(clEnv,CL_DEVICE_TYPE_GPU);
      else if (strcmp(option,"CPU") == 0)
        SelectOpenCLDevice(clEnv,CL_DEVICE_TYPE_CPU);
      else if (strcmp(option,"OFF") == 0)
        {
          for (i = 0; i < clEnv->number_devices; i++)
            clEnv->devices[i]->enabled=MagickFalse;
          clEnv->enabled=MagickFalse;
        }
    }

  if (LoadOpenCLBenchmarks(clEnv) == MagickFalse)
    return;

  benchmark=MagickFalse;
  if (clEnv->cpu_score == MAGICKCORE_OPENCL_UNDEFINED_SCORE)
    benchmark=MagickTrue;
  else
    {
      for (i = 0; i < clEnv->number_devices; i++)
      {
        if (clEnv->devices[i]->score == MAGICKCORE_OPENCL_UNDEFINED_SCORE)
        {
          benchmark=MagickTrue;
          break;
        }
      }
    }

  if (benchmark != MagickFalse)
    BenchmarkOpenCLDevices(clEnv);

  best_score=clEnv->cpu_score;
  for (i = 0; i < clEnv->number_devices; i++)
    best_score=MagickMin(clEnv->devices[i]->score,best_score);

  for (i = 0; i < clEnv->number_devices; i++)
  {
    if (clEnv->devices[i]->score != best_score)
      clEnv->devices[i]->enabled=MagickFalse;
  }
}