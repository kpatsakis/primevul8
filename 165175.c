static void DestroyMagickCLCacheInfo(MagickCLCacheInfo info)
{
  ssize_t
    i;

  for (i=0; i < (ssize_t) info->event_count; i++)
    openCL_library->clReleaseEvent(info->events[i]);
  info->events=(cl_event *) RelinquishMagickMemory(info->events);
  if (info->buffer != (cl_mem) NULL)
    openCL_library->clReleaseMemObject(info->buffer);
  RelinquishSemaphoreInfo(&info->events_semaphore);
  ReleaseOpenCLDevice(info->device);
  RelinquishMagickMemory(info);
}