MagickPrivate cl_command_queue AcquireOpenCLCommandQueue(MagickCLDevice device)
{
  cl_command_queue
    queue;

  cl_command_queue_properties
    properties;

  assert(device != (MagickCLDevice) NULL);
  LockSemaphoreInfo(device->lock);
  if ((device->profile_kernels == MagickFalse) &&
      (device->command_queues_index >= 0))
  {
    queue=device->command_queues[device->command_queues_index--];
    UnlockSemaphoreInfo(device->lock);
  }
  else
  {
    UnlockSemaphoreInfo(device->lock);
    properties=(cl_command_queue_properties) NULL;
    if (device->profile_kernels != MagickFalse)
      properties=CL_QUEUE_PROFILING_ENABLE;
    queue=openCL_library->clCreateCommandQueue(device->context,
      device->deviceID,properties,(cl_int *) NULL);
  }
  return(queue);
}