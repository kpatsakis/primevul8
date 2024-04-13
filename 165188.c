MagickPrivate MagickCLCacheInfo RelinquishMagickCLCacheInfo(
  MagickCLCacheInfo info,const MagickBooleanType relinquish_pixels)
{
  if (info == (MagickCLCacheInfo) NULL)
    return((MagickCLCacheInfo) NULL);
  if (relinquish_pixels != MagickFalse)
    {
      MagickBooleanType
        events_completed;

      ssize_t
        i;

      events_completed=MagickTrue;
      for (i=0; i < (ssize_t)info->event_count; i++)
      {
        cl_int
          event_status;

        cl_uint
          status;

        status=openCL_library->clGetEventInfo(info->events[i],
          CL_EVENT_COMMAND_EXECUTION_STATUS,sizeof(cl_int),&event_status,NULL);
        if ((status == CL_SUCCESS) && (event_status != CL_COMPLETE))
          {
            events_completed=MagickFalse;
            break;
          }
      }
      if (events_completed == MagickFalse)
        openCL_library->clSetEventCallback(info->events[info->event_count-1],
          CL_COMPLETE,&DestroyMagickCLCacheInfoAndPixels,info);
      else
        DestroyMagickCLCacheInfoAndPixels((cl_event) NULL,0,info);
    }
  else
    DestroyMagickCLCacheInfo(info);
  return((MagickCLCacheInfo) NULL);
}