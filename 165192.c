MagickPrivate MagickCLDevice RequestOpenCLDevice(MagickCLEnv clEnv)
{
  MagickCLDevice
    device;

  double
    score,
    best_score;

  size_t
    i;

  if (clEnv == (MagickCLEnv) NULL)
    return((MagickCLDevice) NULL);

  if (clEnv->number_devices == 1)
  {
    if (clEnv->devices[0]->enabled)
      return(clEnv->devices[0]);
    else
      return((MagickCLDevice) NULL);
  }

  device=(MagickCLDevice) NULL;
  best_score=0.0;
  LockSemaphoreInfo(openCL_lock);
  for (i = 0; i < clEnv->number_devices; i++)
  {
    if (clEnv->devices[i]->enabled == MagickFalse)
      continue;

    score=clEnv->devices[i]->score+(clEnv->devices[i]->score*
      clEnv->devices[i]->requested);
    if ((device == (MagickCLDevice) NULL) || (score < best_score))
    {
      device=clEnv->devices[i];
      best_score=score;
    }
  }
  if (device != (MagickCLDevice)NULL)
    device->requested++;
  UnlockSemaphoreInfo(openCL_lock);

  return(device);
}