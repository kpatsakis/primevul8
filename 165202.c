MagickPrivate cl_int SetOpenCLKernelArg(cl_kernel kernel,size_t arg_index,
  size_t arg_size,const void *arg_value)
{
  return(openCL_library->clSetKernelArg(kernel,(cl_uint) arg_index,arg_size,
    arg_value));
}