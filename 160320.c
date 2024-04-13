void usage(const char *p)
{
  printf("Options:\n"
         "-J n  - set parallel job count (default 2)\n"
         "-v    - verbose\n"
         "-w    - use camera white balance\n"
         "-T    - output TIFF instead of PPM\n"
         "-a    - average image for white balance\n");
  exit(1);
}