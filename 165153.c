MagickPrivate void ReleaseOpenCLDevice(MagickCLDevice device)
{
  assert(device != (MagickCLDevice) NULL);
  LockSemaphoreInfo(openCL_lock);
  device->requested--;
  UnlockSemaphoreInfo(openCL_lock);
}