MagickPrivate cl_mem CreateOpenCLBuffer(MagickCLDevice device,
  cl_mem_flags flags,size_t size,void *host_ptr)
{
  return(openCL_library->clCreateBuffer(device->context,flags,size,host_ptr,
    (cl_int *) NULL));
}