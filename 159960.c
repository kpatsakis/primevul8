  virtual void DoRead(void *data, uint32 count, uint64 offset)
  {
    if (parent_stream)
    {
      parent_stream->seek(offset, SEEK_SET);
      parent_stream->read(data, 1, count);
    }
  }