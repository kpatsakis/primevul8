int main(int ac, char *av[])
{
  int i, ret;

  LibRaw RawProcessor;
  if (ac < 2)
  {
    printf("multirender_test - LibRaw %s sample. Performs 4 different "
           "renderings of one file\n"
           " %d cameras supported\n"
           "Usage: %s raw-files....\n",
           LibRaw::version(), LibRaw::cameraCount(), av[0]);
    return 0;
  }

  for (i = 1; i < ac; i++)
  {

    printf("Processing file %s\n", av[i]);

    if ((ret = RawProcessor.open_file(av[i])) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot open_file %s: %s\n", av[i], libraw_strerror(ret));
      continue; // no recycle b/c open file will recycle itself
    }

    if ((ret = RawProcessor.unpack()) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot unpack %s: %s\n", av[i], libraw_strerror(ret));
      continue;
    }
    process_once(RawProcessor, 0, 0, 0, 1, -1, av[i]); // default flip
    process_once(RawProcessor, 1, 0, 1, 2, -1, av[i]);
    process_once(RawProcessor, 1, 1, 0, 3, -1, av[i]); // default flip
    process_once(RawProcessor, 1, 1, 0, 4, 1, av[i]);  // flip 1
    process_once(RawProcessor, 1, 1, 0, 5, 3, av[i]);  // flip 3
    process_once(RawProcessor, 1, 1, 0, 6, 1, av[i]);  // 1 again same as 4
    process_once(RawProcessor, 1, 1, 0, 7, -1,
                 av[i]); // default again, same as 3
    process_once(RawProcessor, 0, 0, 0, 8, -1, av[i]); // same as 1

    RawProcessor.recycle(); // just for show this call
  }
  return 0;
}