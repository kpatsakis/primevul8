LibRaw_file_datastream::LibRaw_file_datastream(const char *fname)
    : filename(fname), _fsize(0)
#ifdef LIBRAW_WIN32_UNICODEPATHS
      ,
      wfilename()
#endif
      ,
      jas_file(NULL)
{
  if (filename.size() > 0)
  {
#ifndef LIBRAW_WIN32_CALLS
    struct stat st;
    if (!stat(filename.c_str(), &st))
      _fsize = st.st_size;
#else
    struct _stati64 st;
    if (!_stati64(filename.c_str(), &st))
      _fsize = st.st_size;
#endif
#ifdef LIBRAW_USE_AUTOPTR
    std::auto_ptr<std::filebuf> buf(new std::filebuf());
#else
    std::unique_ptr<std::filebuf> buf(new std::filebuf());
#endif
    buf->open(filename.c_str(), std::ios_base::in | std::ios_base::binary);
    if (buf->is_open())
    {
#ifdef LIBRAW_USE_AUTOPTR
      f = buf;
#else
      f = std::move(buf);
#endif
    }
  }
}