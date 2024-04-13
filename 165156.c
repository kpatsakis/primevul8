static inline void RelinquishMagickCLDevices(MagickCLEnv clEnv)
{
  size_t
    i;

  if (clEnv->devices != (MagickCLDevice *) NULL)
    {
      for (i = 0; i < clEnv->number_devices; i++)
        clEnv->devices[i]=RelinquishMagickCLDevice(clEnv->devices[i]);
      clEnv->devices=(MagickCLDevice *) RelinquishMagickMemory(clEnv->devices);
    }
  clEnv->number_devices=0;
}