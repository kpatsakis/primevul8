void *LibRaw_bigfile_datastream::make_jas_stream()
{
#ifdef NO_JASPER
  return NULL;
#else
  return jas_stream_fdopen(fileno(f), "rb");
#endif
}