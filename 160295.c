int main(int ac, char *av[])
{
  int i;
  libraw_data_t *iprc = libraw_init(0);

  if (!iprc)
  {
    fprintf(stderr, "Cannot create libraw handle\n");
    exit(1);
  }

  iprc->params.half_size = 1; /* dcraw -h */

  for (i = 1; i < ac; i++)
  {
    char outfn[1024];
    int ret = libraw_open_file(iprc, av[i]);
    HANDLE_ALL_ERRORS(ret);

    printf("Processing %s (%s %s)\n", av[i], iprc->idata.make,
           iprc->idata.model);

    ret = libraw_unpack(iprc);
    HANDLE_ALL_ERRORS(ret);

    ret = libraw_dcraw_process(iprc);
    HANDLE_ALL_ERRORS(ret);

    strcpy(outfn, av[i]);
    strcat(outfn, ".ppm");
    printf("Writing to %s\n", outfn);

    ret = libraw_dcraw_ppm_tiff_writer(iprc, outfn);
    HANDLE_FATAL_ERROR(ret);
  }
  libraw_close(iprc);
  return 0;
}