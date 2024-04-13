void *process_files(void *q)
{
  int ret;
  int count = 0;
  char outfn[1024], *fn;
  libraw_data_t *iprc = libraw_init(0);

  if (!iprc)
  {
    fprintf(stderr, "Cannot create libraw handle\n");
    return NULL;
  }

  while ((fn = get_next_file()))
  {

    iprc->params.half_size = 1; /* dcraw -h */
    iprc->params.use_camera_wb = use_camera_wb;
    iprc->params.use_auto_wb = use_auto_wb;
    iprc->params.output_tiff = tiff_mode;

    ret = libraw_open_file(iprc, fn);
    if (verbose)
      fprintf(stderr, "%s: %s/%s\n", fn, iprc->idata.make, iprc->idata.model);
    HANDLE_ERRORS(ret);

    ret = libraw_unpack(iprc);
    HANDLE_ERRORS(ret);

    ret = libraw_dcraw_process(iprc);
    HANDLE_ERRORS(ret);

    snprintf(outfn, 1023, "%s.%s", fn, tiff_mode ? "tiff" : "ppm");

    if (verbose)
      fprintf(stderr, "Writing file %s\n", outfn);
    ret = libraw_dcraw_ppm_tiff_writer(iprc, outfn);
    HANDLE_ERRORS(ret);
    count++;
  }
  libraw_close(iprc);
  return NULL;
}