static MagickBooleanType LoadCachedOpenCLKernel(MagickCLDevice device,
  const char *filename)
{
  cl_int
    binaryStatus,
    status;

  ExceptionInfo
    *exception;

  size_t
    length;

  unsigned char
    *binaryProgram;

  exception=AcquireExceptionInfo();
  binaryProgram=(unsigned char *) FileToBlob(filename,~0UL,&length,exception);
  exception=DestroyExceptionInfo(exception);
  if (binaryProgram == (unsigned char *) NULL)
    return(MagickFalse);
  device->program=openCL_library->clCreateProgramWithBinary(device->context,1,
    &device->deviceID,&length,(const unsigned char**)&binaryProgram,
    &binaryStatus,&status);
  binaryProgram=(unsigned char *) RelinquishMagickMemory(binaryProgram);
  return((status != CL_SUCCESS) || (binaryStatus != CL_SUCCESS) ? MagickFalse :
    MagickTrue);
}