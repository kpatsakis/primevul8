MagickPrivate void ReleaseOpenCLMemObject(cl_mem memobj)
{
  (void) openCL_library->clReleaseMemObject(memobj);
}