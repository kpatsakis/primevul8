static cl_uint GetOpenCLDeviceCount(MagickCLEnv clEnv,cl_platform_id platform)
{
  char
    version[MagickPathExtent];

  cl_uint
    num;

  if (clEnv->library->clGetPlatformInfo(platform,CL_PLATFORM_VERSION,
        MagickPathExtent,version,NULL) != CL_SUCCESS)
    return(0);
  if (strncmp(version, "OpenCL 1.0 ", 11) == 0)
    return(0);
  if (clEnv->library->clGetDeviceIDs(platform,
        CL_DEVICE_TYPE_CPU|CL_DEVICE_TYPE_GPU,0,NULL,&num) != CL_SUCCESS)
    return(0);
  return(num);
}