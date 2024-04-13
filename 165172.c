MagickPrivate MagickCLCacheInfo CopyMagickCLCacheInfo(MagickCLCacheInfo info)
{
  cl_command_queue
    queue;

  cl_event
    *events;

  cl_uint
    event_count;

  Quantum
    *pixels;

  if (info == (MagickCLCacheInfo) NULL)
    return((MagickCLCacheInfo) NULL);
  events=CopyOpenCLEvents(info,(MagickCLCacheInfo) NULL,&event_count);
  if (events != (cl_event *) NULL)
    {
      queue=AcquireOpenCLCommandQueue(info->device);
      pixels=openCL_library->clEnqueueMapBuffer(queue,info->buffer,CL_TRUE,
        CL_MAP_READ | CL_MAP_WRITE,0,info->length,event_count,events,
        (cl_event *) NULL,(cl_int *) NULL);
      assert(pixels == info->pixels);
      ReleaseOpenCLCommandQueue(info->device,queue);
      events=(cl_event *) RelinquishMagickMemory(events);
    }
  return(RelinquishMagickCLCacheInfo(info,MagickFalse));
}