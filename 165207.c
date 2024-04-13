MagickPrivate cl_kernel AcquireOpenCLKernel(MagickCLDevice device,
  const char *kernel_name)
{
  cl_kernel
    kernel;

  assert(device != (MagickCLDevice) NULL);
  kernel=openCL_library->clCreateKernel(device->program,kernel_name,
    (cl_int *) NULL);
  return(kernel);
}