MagickPrivate void ReleaseOpenCLKernel(cl_kernel kernel)
{
  (void) openCL_library->clReleaseKernel(kernel);
}