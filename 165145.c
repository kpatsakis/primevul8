static cl_event* CopyOpenCLEvents(MagickCLCacheInfo first,
  MagickCLCacheInfo second,cl_uint *event_count)
{
  cl_event
    *events;

  register size_t
    i;

  size_t
    j;

  assert(first != (MagickCLCacheInfo) NULL);
  assert(event_count != (cl_uint *) NULL);
  events=(cl_event *) NULL;
  LockSemaphoreInfo(first->events_semaphore);
  if (second != (MagickCLCacheInfo) NULL)
    LockSemaphoreInfo(second->events_semaphore);
  *event_count=first->event_count;
  if (second != (MagickCLCacheInfo) NULL)
    *event_count+=second->event_count;
  if (*event_count > 0)
    {
      events=AcquireQuantumMemory(*event_count,sizeof(*events));
      if (events == (MagickCLCacheInfo) NULL)
        *event_count=0;
      else
        {
          j=0;
          for (i=0; i < first->event_count; i++, j++)
            events[j]=first->events[i];
          if (second != (MagickCLCacheInfo) NULL)
            {
              for (i=0; i < second->event_count; i++, j++)
                events[j]=second->events[i];
            }
        }
    }
  UnlockSemaphoreInfo(first->events_semaphore);
  if (second != (MagickCLCacheInfo) NULL)
    UnlockSemaphoreInfo(second->events_semaphore);
  return(events);
}