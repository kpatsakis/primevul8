LibRaw_bigfile_datastream::LibRaw_bigfile_datastream(const char *fname)
    : filename(fname)
#ifdef LIBRAW_WIN32_UNICODEPATHS
      ,
      wfilename()
#endif
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

#ifndef WIN32SECURECALLS
    f = fopen(fname, "rb");
#else
    if (fopen_s(&f, fname, "rb"))
      f = 0;
#endif
  }
  else
  {
    filename = std::string();
    f = 0;
  }
}