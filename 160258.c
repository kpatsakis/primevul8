  libraw_dng_stream(LibRaw_abstract_datastream *p)
      : dng_stream((dng_abort_sniffer *)NULL, kBigBufferSize, 0),
        parent_stream(p)
  {
    if (parent_stream)
    {
      off = parent_stream->tell();
      parent_stream->seek(0UL, SEEK_SET); /* seek to start */
    }
  }