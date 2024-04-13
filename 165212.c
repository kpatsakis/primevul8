static MagickCLDevice RelinquishMagickCLDevice(MagickCLDevice device)
{
  if (device == (MagickCLDevice) NULL)
    return((MagickCLDevice) NULL);

  device->platform_name=RelinquishMagickMemory(device->platform_name);
  device->vendor_name=RelinquishMagickMemory(device->vendor_name);
  device->name=RelinquishMagickMemory(device->name);
  device->version=RelinquishMagickMemory(device->version);
  if (device->program != (cl_program) NULL)
    (void) openCL_library->clReleaseProgram(device->program);
  while (device->command_queues_index >= 0)
    (void) openCL_library->clReleaseCommandQueue(
      device->command_queues[device->command_queues_index--]);
  RelinquishSemaphoreInfo(&device->lock);
  return((MagickCLDevice) RelinquishMagickMemory(device));
}