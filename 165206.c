MagickExport const KernelProfileRecord *GetOpenCLKernelProfileRecords(
  const MagickCLDevice device,size_t *length)
{
  if ((device == (const MagickCLDevice) NULL) || (device->profile_records ==
      (KernelProfileRecord *) NULL))
  {
    if (length != (size_t *) NULL)
      *length=0;
    return((const KernelProfileRecord *) NULL);
  }
  if (length != (size_t *) NULL)
    {
      *length=0;
      LockSemaphoreInfo(device->lock);
      while (device->profile_records[*length] != (KernelProfileRecord) NULL)
        *length=*length+1;
      UnlockSemaphoreInfo(device->lock);
    }
  return(device->profile_records);
}