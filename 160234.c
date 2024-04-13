int LibRaw_bigfile_datastream::jpeg_src(void *jpegdata)
{
#ifdef NO_JPEG
  return -1;
#else
  if (!f)
    return -1;
  j_decompress_ptr cinfo = (j_decompress_ptr)jpegdata;
  jpeg_stdio_src(cinfo, f);
  return 0; // OK
#endif
}