int process_once(LibRaw &RawProcessor, int half_mode, int camera_wb,
                 int auto_wb, int suffix, int user_flip, char *fname)
{
  char outfn[1024];
  RawProcessor.imgdata.params.half_size = half_mode;
  RawProcessor.imgdata.params.use_camera_wb = camera_wb;
  RawProcessor.imgdata.params.use_auto_wb = auto_wb;
  RawProcessor.imgdata.params.user_flip = user_flip;

  int ret = RawProcessor.dcraw_process();

  if (LIBRAW_SUCCESS != ret)
  {
    fprintf(stderr, "Cannot do postpocessing on %s: %s\n", fname,
            libraw_strerror(ret));
    return ret;
  }
  snprintf(outfn, sizeof(outfn), "%s.%d.%s", fname, suffix,
           (RawProcessor.imgdata.idata.colors > 1 ? "ppm" : "pgm"));

  printf("Writing file %s\n", outfn);

  if (LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_ppm_tiff_writer(outfn)))
    fprintf(stderr, "Cannot write %s: %s\n", outfn, libraw_strerror(ret));
  return ret;
}