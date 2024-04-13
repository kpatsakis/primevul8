  ~libraw_dng_stream()
  {
    if (parent_stream)
      parent_stream->seek(off, SEEK_SET);
  }