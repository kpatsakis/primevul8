static void SelectOpenCLDevice(MagickCLEnv clEnv,cl_device_type type)
{
  MagickCLDevice
    device;

  size_t
    i,
    j;

  for (i = 0; i < clEnv->number_devices; i++)
    clEnv->devices[i]->enabled=MagickFalse;

  for (i = 0; i < clEnv->number_devices; i++)
  {
    device=clEnv->devices[i];
    if (device->type != type)
      continue;

    device->enabled=MagickTrue;
    for (j = i+1; j < clEnv->number_devices; j++)
    {
      MagickCLDevice
        other_device;

      other_device=clEnv->devices[j];
      if (IsSameOpenCLDevice(device,other_device))
        other_device->enabled=MagickTrue;
    }
  }
}