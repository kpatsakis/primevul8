int LibRaw_buffer_datastream::seek(INT64 o, int whence)
{
  switch (whence)
  {
  case SEEK_SET:
    if (o < 0)
      streampos = 0;
    else if (size_t(o) > streamsize)
      streampos = streamsize;
    else
      streampos = size_t(o);
    return 0;
  case SEEK_CUR:
    if (o < 0)
    {
      if (size_t(-o) >= streampos)
        streampos = 0;
      else
        streampos += (size_t)o;
    }
    else if (o > 0)
    {
      if (o + streampos > streamsize)
        streampos = streamsize;
      else
        streampos += (size_t)o;
    }
    return 0;
  case SEEK_END:
    if (o > 0)
      streampos = streamsize;
    else if (size_t(-o) > streamsize)
      streampos = 0;
    else
      streampos = streamsize + (size_t)o;
    return 0;
  default:
    return 0;
  }
}