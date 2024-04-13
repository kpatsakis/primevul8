int LibRaw_file_datastream::jpeg_src(void *jpegdata)
{
#ifdef NO_JPEG
  return -1; // not supported
#else
  if (jas_file)
  {
    fclose(jas_file);
    jas_file = NULL;
  }
#ifdef LIBRAW_WIN32_UNICODEPATHS
  if (wfname())
  {
    jas_file = _wfopen(wfname(), L"rb");
  }
  else
#endif
  {
    jas_file = fopen(fname(), "rb");
  }
  if (jas_file)
  {
    fseek(jas_file, tell(), SEEK_SET);
    j_decompress_ptr cinfo = (j_decompress_ptr)jpegdata;
    jpeg_stdio_src(cinfo, jas_file);
    return 0; // OK
  }
  return -1;
#endif
}