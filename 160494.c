int main(int ac, char *av[])
{
  int i, max_threads = 2;
  pthread_t *threads;
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
  pthread_mutex_init(&qm, NULL);
  threads = calloc(max_threads, sizeof(threads[0]));
  for (i = 0; i < max_threads; i++)
    pthread_create(&threads[i], NULL, process_files, NULL);
  for (i = 0; i < max_threads; i++)
  {
    int *iptr;
    if (threads[i])
    {
      pthread_join(threads[i], (void *)&iptr);
    }
  }

  return 0;
}