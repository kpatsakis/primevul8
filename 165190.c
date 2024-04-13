static MagickBooleanType BindOpenCLFunctions()
{
#ifdef MAGICKCORE_OPENCL_MACOSX
#define BIND(X) openCL_library->X= &X;
#else
  (void) ResetMagickMemory(openCL_library,0,sizeof(MagickLibrary));
#ifdef MAGICKCORE_WINDOWS_SUPPORT
  openCL_library->library=(void *)LoadLibraryA("OpenCL.dll");
#else
  openCL_library->library=(void *)dlopen("libOpenCL.so", RTLD_NOW);
#endif
#define BIND(X) \
  if ((openCL_library->X=(MAGICKpfn_##X)OsLibraryGetFunctionAddress(openCL_library->library,#X)) == NULL) \
    return(MagickFalse);
#endif

  if (openCL_library->library == (void*) NULL)
    return(MagickFalse);

  BIND(clGetPlatformIDs);
  BIND(clGetPlatformInfo);

  BIND(clGetDeviceIDs);
  BIND(clGetDeviceInfo);

  BIND(clCreateBuffer);
  BIND(clReleaseMemObject);

  BIND(clCreateContext);
  BIND(clReleaseContext);

  BIND(clCreateCommandQueue);
  BIND(clReleaseCommandQueue);
  BIND(clFlush);
  BIND(clFinish);

  BIND(clCreateProgramWithSource);
  BIND(clCreateProgramWithBinary);
  BIND(clReleaseProgram);
  BIND(clBuildProgram);
  BIND(clGetProgramBuildInfo);
  BIND(clGetProgramInfo);

  BIND(clCreateKernel);
  BIND(clReleaseKernel);
  BIND(clSetKernelArg);
  BIND(clGetKernelInfo);

  BIND(clEnqueueReadBuffer);
  BIND(clEnqueueMapBuffer);
  BIND(clEnqueueUnmapMemObject);
  BIND(clEnqueueNDRangeKernel);

  BIND(clGetEventInfo);
  BIND(clWaitForEvents);
  BIND(clReleaseEvent);
  BIND(clRetainEvent);
  BIND(clSetEventCallback);

  BIND(clGetEventProfilingInfo);

  return(MagickTrue);
}