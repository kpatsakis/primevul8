char *get_next_file()
{
  char *ret;
  DWORD dwWaitResult;
  if (!queue)
    return NULL;
  if (qptr >= qsize)
    return NULL;

  dwWaitResult = WaitForSingleObject(qmutex,    // handle to mutex
                                     INFINITE); // no time-out interval
  switch (dwWaitResult)
  {
  // The thread got ownership of the mutex
  case WAIT_OBJECT_0:
    ret = queue[qptr++];
    ReleaseMutex(qmutex);
    break;
  case WAIT_ABANDONED:
    return NULL; // cannot obtain the lock
  };
  return ret;
}