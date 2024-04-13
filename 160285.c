LibRaw_buffer_datastream::LibRaw_buffer_datastream(void *buffer, size_t bsize)
{
  buf = (unsigned char *)buffer;
  streampos = 0;
  streamsize = bsize;
}