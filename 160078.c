  virtual uint64 DoGetLength()
  {
    if (parent_stream)
      return parent_stream->size();
    return 0;
  }