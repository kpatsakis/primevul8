static void RegisterCacheEvent(MagickCLCacheInfo info,cl_event event)
{
  assert(info != (MagickCLCacheInfo) NULL);
  assert(event != (cl_event) NULL);
  LockSemaphoreInfo(info->events_semaphore);
  if (info->events == (cl_event *) NULL)
    {
      info->events=AcquireMagickMemory(sizeof(*info->events));
      info->event_count=1;
    }
  else
    info->events=ResizeQuantumMemory(info->events,++info->event_count,
      sizeof(*info->events));
  if (info->events == (cl_event *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  info->events[info->event_count-1]=event;
  UnlockSemaphoreInfo(info->events_semaphore);
  openCL_library->clRetainEvent(event);
}