MagickPrivate MagickBooleanType EnqueueOpenCLKernel(cl_command_queue queue,
  cl_kernel kernel,cl_uint work_dim,const size_t *offset,const size_t *gsize,
  const size_t *lsize,const Image *input_image,const Image *output_image,
  MagickBooleanType flush,ExceptionInfo *exception)
{
  CacheInfo
    *output_info,
    *input_info;

  cl_event
    event,
    *events;

  cl_int
    status;

  cl_uint
    event_count;

  assert(input_image != (const Image *) NULL);
  input_info=(CacheInfo *) input_image->cache;
  assert(input_info != (CacheInfo *) NULL);
  assert(input_info->opencl != (MagickCLCacheInfo) NULL);
  output_info=(CacheInfo *) NULL;
  if (output_image == (const Image *) NULL)
    events=CopyOpenCLEvents(input_info->opencl,(MagickCLCacheInfo) NULL,
      &event_count);
  else
    {
      output_info=(CacheInfo *) output_image->cache;
      assert(output_info != (CacheInfo *) NULL);
      assert(output_info->opencl != (MagickCLCacheInfo) NULL);
      events=CopyOpenCLEvents(input_info->opencl,output_info->opencl,
        &event_count);
    }
  status=openCL_library->clEnqueueNDRangeKernel(queue,kernel,work_dim,offset,
    gsize,lsize,event_count,events,&event);
  /* This can fail due to memory issues and calling clFinish might help. */
  if ((status != CL_SUCCESS) && (event_count > 0))
    {
      openCL_library->clFinish(queue);
      status=openCL_library->clEnqueueNDRangeKernel(queue,kernel,work_dim,
        offset,gsize,lsize,event_count,events,&event);
    }
  events=(cl_event *) RelinquishMagickMemory(events);
  if (status != CL_SUCCESS)
    {
      (void) OpenCLThrowMagickException(input_info->opencl->device,exception,
        GetMagickModule(),ResourceLimitWarning,
        "clEnqueueNDRangeKernel failed.","'%s'",".");
      return(MagickFalse);
    }
  if (flush != MagickFalse)
    openCL_library->clFlush(queue);
  if (RecordProfileData(input_info->opencl->device,kernel,event) == MagickFalse)
    {
      RegisterCacheEvent(input_info->opencl,event);
      if (output_info != (CacheInfo *) NULL)
        RegisterCacheEvent(output_info->opencl,event);
    }
  openCL_library->clReleaseEvent(event);
  return(MagickTrue);
}