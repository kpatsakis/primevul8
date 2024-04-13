  libraw_data_t *libraw_init(unsigned int flags)
  {
    LibRaw *ret;
    try
    {
      ret = new LibRaw(flags);
    }
    catch (std::bad_alloc)
    {
      return NULL;
    }
    return &(ret->imgdata);
  }