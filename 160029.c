int my_progress_callback(void *d, enum LibRaw_progress p, int iteration,
                         int expected)
{
  char *passed = (char *)(d ? d : "default string"); // data passed to callback
                                                     // at set_callback stage

  if (verbosity > 2) // verbosity set by repeat -v switches
  {
    printf("CB: %s  pass %d of %d (data passed=%s)\n", libraw_strprogress(p),
           iteration, expected, passed);
  }
  else if (iteration == 0) // 1st iteration of each step
    printf("Starting %s (expecting %d iterations)\n", libraw_strprogress(p),
           expected);
  else if (iteration == expected - 1)
    printf("%s finished\n", libraw_strprogress(p));

  ///    if(++cnt>10) return 1; // emulate user termination on 10-th callback
  ///    call

  return 0; // always return 0 to continue processing
}