MagickPrivate MagickCLCacheInfo AcquireMagickCLCacheInfo(MagickCLDevice device,
  Quantum *pixels,const MagickSizeType length)
{
  cl_int
    status;

  MagickCLCacheInfo
    info;

  info=(MagickCLCacheInfo) AcquireMagickMemory(sizeof(*info));
  if (info == (MagickCLCacheInfo) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) ResetMagickMemory(info,0,sizeof(*info));
  LockSemaphoreInfo(openCL_lock);
  device->requested++;
  UnlockSemaphoreInfo(openCL_lock);
  info->device=device;
  info->length=length;
  info->pixels=pixels;
  info->events_semaphore=AcquireSemaphoreInfo();
  info->buffer=openCL_library->clCreateBuffer(device->context,
    CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,(size_t) length,(void *) pixels,
    &status);
  if (status == CL_SUCCESS)
    return(info);
  LockSemaphoreInfo(openCL_lock);
  device->requested--;
  UnlockSemaphoreInfo(openCL_lock);
  return((MagickCLCacheInfo) RelinquishMagickMemory(info));
}