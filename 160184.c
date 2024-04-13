void *LibRaw_file_datastream::make_jas_stream()
{
#ifdef NO_JASPER
  return NULL;
#else
#ifdef LIBRAW_WIN32_UNICODEPATHS
  if (wfname())
  {
    jas_file = _wfopen(wfname(), L"rb");
    return jas_stream_fdopen(fileno(jas_file), "rb");
  }
  else
#endif
  {
    return jas_stream_fopen(fname(), "rb");
  }
#endif
}