static void RunDeviceBenckmark(MagickCLEnv clEnv,MagickCLEnv testEnv,
  MagickCLDevice device)
{
  testEnv->devices[0]=device;
  default_CLEnv=testEnv;
  device->score=RunOpenCLBenchmark(MagickFalse);
  default_CLEnv=clEnv;
  testEnv->devices[0]=(MagickCLDevice) NULL;
}