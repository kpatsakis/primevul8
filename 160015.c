void usage(const char *p)
{
  printf("%s: Multi-threaded LibRaw sample app. Emulates dcraw -h [-w] [-a]\n",
         p);
  printf("Options:\n"
         "-J n  - set parallel job count (default 2)\n"
         "-v    - verbose\n"
         "-w    - use camera white balance\n"
         "-a    - average image for white balance\n");
  exit(1);
}