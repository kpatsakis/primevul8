static void BenchmarkOpenCLDevices(MagickCLEnv clEnv)
{
  MagickCLDevice
    device;

  MagickCLEnv
    testEnv;

  size_t
    i,
    j;

  testEnv=AcquireMagickCLEnv();
  testEnv->library=openCL_library;
  testEnv->devices=(MagickCLDevice *) AcquireCriticalMemory(
    sizeof(MagickCLDevice));
  testEnv->number_devices=1;
  testEnv->benchmark_thread_id=GetMagickThreadId();
  testEnv->initialized=MagickTrue;

  for (i = 0; i < clEnv->number_devices; i++)
    clEnv->devices[i]->score=MAGICKCORE_OPENCL_UNDEFINED_SCORE;

  for (i = 0; i < clEnv->number_devices; i++)
  {
    device=clEnv->devices[i];
    if (device->score == MAGICKCORE_OPENCL_UNDEFINED_SCORE)
      RunDeviceBenckmark(clEnv,testEnv,device);

    /* Set the score on all the other devices that are the same */
    for (j = i+1; j < clEnv->number_devices; j++)
    {
      MagickCLDevice
        other_device;

      other_device=clEnv->devices[j];
      if (IsSameOpenCLDevice(device,other_device))
        other_device->score=device->score;
    }
  }

  testEnv->enabled=MagickFalse;
  default_CLEnv=testEnv;
  clEnv->cpu_score=RunOpenCLBenchmark(MagickTrue);
  default_CLEnv=clEnv;

  testEnv=RelinquishMagickCLEnv(testEnv);
  CacheOpenCLBenchmarks(clEnv);
}