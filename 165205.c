static MagickBooleanType HasOpenCLDevices(MagickCLEnv clEnv,
  ExceptionInfo *exception)
{
  char
    *accelerateKernelsBuffer,
    options[MagickPathExtent];

  MagickStatusType
    status;

  size_t
    i;

  size_t
    signature;

  /* Check if there are enabled devices */
  for (i = 0; i < clEnv->number_devices; i++)
  {
    if ((clEnv->devices[i]->enabled != MagickFalse))
      break;
  }
  if (i == clEnv->number_devices)
    return(MagickFalse);

  /* Check if we need to compile a kernel for one of the devices */
  status=MagickTrue;
  for (i = 0; i < clEnv->number_devices; i++)
  {
    if ((clEnv->devices[i]->enabled != MagickFalse) &&
        (clEnv->devices[i]->program == (cl_program) NULL))
    {
      status=MagickFalse;
      break;
    }
  }
  if (status != MagickFalse)
    return(MagickTrue);

  /* Get additional options */
  (void) FormatLocaleString(options,MaxTextExtent,CLOptions,
    (float)QuantumRange,(float)QuantumScale,(float)CLCharQuantumScale,
    (float)MagickEpsilon,(float)MagickPI,(unsigned int)MaxMap,
    (unsigned int)MAGICKCORE_QUANTUM_DEPTH);

  signature=StringSignature(options);
  accelerateKernelsBuffer=(char*) AcquireMagickMemory(
    strlen(accelerateKernels)+strlen(accelerateKernels2)+1);
  if (accelerateKernelsBuffer == (char*) NULL)
    return(MagickFalse);
  sprintf(accelerateKernelsBuffer,"%s%s",accelerateKernels,accelerateKernels2);
  signature^=StringSignature(accelerateKernelsBuffer);

  status=MagickTrue;
  for (i = 0; i < clEnv->number_devices; i++)
  {
    MagickCLDevice
      device;

    size_t
      device_signature;

    device=clEnv->devices[i];
    if ((device->enabled == MagickFalse) ||
        (device->program != (cl_program) NULL))
      continue;

    LockSemaphoreInfo(device->lock);
    if (device->program != (cl_program) NULL)
    {
      UnlockSemaphoreInfo(device->lock);
      continue;
    }
    device_signature=signature;
    device_signature^=StringSignature(device->platform_name);
    status=CompileOpenCLKernel(device,accelerateKernelsBuffer,options,
      device_signature,exception);
    UnlockSemaphoreInfo(device->lock);
    if (status == MagickFalse)
      break;
  }
  accelerateKernelsBuffer=RelinquishMagickMemory(accelerateKernelsBuffer);
  return(status);
}