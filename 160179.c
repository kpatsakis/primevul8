int LibRaw::dcraw_ppm_tiff_writer(const char *filename)
{
  CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

  if (!imgdata.image)
    return LIBRAW_OUT_OF_ORDER_CALL;

  if (!filename)
    return ENOENT;
  FILE *f = NULL;
  if (!strcmp(filename, "-"))
  {
#ifdef LIBRAW_WIN32_CALLS
    _setmode(_fileno(stdout), _O_BINARY);
#endif
    f = stdout;
  }
  else
    f = fopen(filename, "wb");

  if (!f)
    return errno;

  try
  {
    if (!libraw_internal_data.output_data.histogram)
    {
      libraw_internal_data.output_data.histogram =
          (int(*)[LIBRAW_HISTOGRAM_SIZE])malloc(
              sizeof(*libraw_internal_data.output_data.histogram) * 4);
      merror(libraw_internal_data.output_data.histogram,
             "LibRaw::dcraw_ppm_tiff_writer()");
    }
    libraw_internal_data.internal_data.output = f;
    write_ppm_tiff();
    SET_PROC_FLAG(LIBRAW_PROGRESS_FLIP);
    libraw_internal_data.internal_data.output = NULL;
    if (strcmp(filename, "-"))
      fclose(f);
    return 0;
  }
  catch (LibRaw_exceptions err)
  {
    if (strcmp(filename, "-"))
      fclose(f);
    EXCEPTION_HANDLER(err);
  }
}