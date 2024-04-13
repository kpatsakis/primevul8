void *LibRaw_buffer_datastream::make_jas_stream()
{
#ifdef NO_JASPER
  return NULL;
#else
  return jas_stream_memopen((char *)buf + streampos, streamsize - streampos);
#endif
}