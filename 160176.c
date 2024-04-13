int main(int ac, char *av[])
{
  int i, max_threads = 2;
  HANDLE *threads;
  DWORD ThreadID;

  if (ac < 2)
    usage(av[0]);

  queue = calloc(ac - 1, sizeof(queue[0]));

  for (i = 1; i < ac; i++)
  {
    if (av[i][0] == '-')
    {
      if (av[i][1] == 'w')
        use_camera_wb = 1;
      if (av[i][1] == 'a')
        use_auto_wb = 1;
      if (av[i][1] == 'v')
        verbose = 1;
      if (av[i][1] == 'T')
        tiff_mode = 1;
      if (av[i][1] == 'J')
      {
        max_threads = atoi(av[++i]);
        if (max_threads < 1)
        {
          fprintf(stderr, "Job count should be at least 1\n");
          exit(1);
        }
      }
    }
    else
      queue[qsize++] = av[i];
  }
  qmutex = CreateMutex(NULL, FALSE, NULL);
  threads = calloc(max_threads, sizeof(threads[0]));
  for (i = 0; i < max_threads; i++)
  {

    if (NULL ==
        (threads[i] = CreateThread(NULL, // default security attributes
                                   0,    // default stack size
                                   (LPTHREAD_START_ROUTINE)process_files,
                                   NULL,      // no thread function arguments
                                   0,         // default creation flags
                                   &ThreadID) // receive thread identifier
         ))
    {
      printf("CreateThread error: %d\n", GetLastError());
      return 1;
    }
  }

  WaitForMultipleObjects(max_threads, threads, TRUE, INFINITE);

  // Close thread and mutex handles

  for (i = 0; i < max_threads; i++)
    CloseHandle(threads[i]);

  CloseHandle(qmutex);

  return 0;
}