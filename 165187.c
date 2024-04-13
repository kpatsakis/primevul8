MagickPrivate void ReleaseOpenCLCommandQueue(MagickCLDevice device,
  cl_command_queue queue)
{
  if (queue == (cl_command_queue) NULL)
    return;

  assert(device != (MagickCLDevice) NULL);
  LockSemaphoreInfo(device->lock);
  if ((device->profile_kernels != MagickFalse) ||
      (device->command_queues_index >= MAGICKCORE_OPENCL_COMMAND_QUEUES-1))
    {
      UnlockSemaphoreInfo(device->lock);
      openCL_library->clFinish(queue);
      (void) openCL_library->clReleaseCommandQueue(queue);
    }
  else
    {
      openCL_library->clFlush(queue);
      device->command_queues[++device->command_queues_index]=queue;
      UnlockSemaphoreInfo(device->lock);
    }
}