static MagickBooleanType CompileOpenCLKernel(MagickCLDevice device,
  const char *kernel,const char *options,size_t signature,
  ExceptionInfo *exception)
{
  char
    deviceName[MagickPathExtent],
    filename[MagickPathExtent],
    *ptr;

  cl_int
    status;

  MagickBooleanType
    loaded;

  size_t
    length;

  (void) CopyMagickString(deviceName,device->name,MagickPathExtent);
  ptr=deviceName;
  /* Strip out illegal characters for file names */
  while (*ptr != '\0')
  {
    if ((*ptr == ' ') || (*ptr == '\\') || (*ptr == '/') || (*ptr == ':') ||
        (*ptr == '*') || (*ptr == '?') || (*ptr == '"') || (*ptr == '<') ||
        (*ptr == '>' || *ptr == '|'))
      *ptr = '_';
    ptr++;
  }
  (void) FormatLocaleString(filename,MagickPathExtent,
    "%s%s%s_%s_%08x_%.20g.bin",GetOpenCLCacheDirectory(),
    DirectorySeparator,"magick_opencl",deviceName,signature,
    (double) sizeof(char*)*8);
  loaded=LoadCachedOpenCLKernel(device,filename);
  if (loaded == MagickFalse)
    {
      /* Binary CL program unavailable, compile the program from source */
      length=strlen(kernel);
      device->program=openCL_library->clCreateProgramWithSource(
        device->context,1,&kernel,&length,&status);
      if (status != CL_SUCCESS)
        return(MagickFalse);
    }

  status=openCL_library->clBuildProgram(device->program,1,&device->deviceID,
    options,NULL,NULL);
  if (status != CL_SUCCESS)
  {
    (void) ThrowMagickException(exception,GetMagickModule(),DelegateWarning,
      "clBuildProgram failed.","(%d)",(int)status);
    LogOpenCLBuildFailure(device,kernel,exception);
    return(MagickFalse);
  }

  /* Save the binary to a file to avoid re-compilation of the kernels */
  if (loaded == MagickFalse)
    CacheOpenCLKernel(device,filename,exception);

  return(MagickTrue);
}