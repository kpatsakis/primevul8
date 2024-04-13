int LibRaw_buffer_datastream::eof()
{
  return streampos >= streamsize;
}