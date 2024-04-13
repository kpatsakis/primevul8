LibRaw_file_datastream::LibRaw_file_datastream(const wchar_t *fname)
    : filename(), wfilename(fname), jas_file(NULL), _fsize(0)
{
  if (wfilename.size() > 0)
  {
    struct _stati64 st;
    if (!_wstati64(wfilename.c_str(), &st))
      _fsize = st.st_size;
#ifdef LIBRAW_USE_AUTOPTR
    std::auto_ptr<std::filebuf> buf(new std::filebuf());
#else
    std::unique_ptr<std::filebuf> buf(new std::filebuf());
#endif
    buf->open(wfilename.c_str(), std::ios_base::in | std::ios_base::binary);
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