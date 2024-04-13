static MagickCLDevice AcquireMagickCLDevice()
{
  MagickCLDevice
    device;

  device=(MagickCLDevice) AcquireMagickMemory(sizeof(*device));
  if (device != NULL)
  {
    (void) ResetMagickMemory(device,0,sizeof(*device));
    ActivateSemaphoreInfo(&device->lock);
    device->score=MAGICKCORE_OPENCL_UNDEFINED_SCORE;
    device->command_queues_index=-1;
    device->enabled=MagickTrue;
  }
  return(device);
}