LibRaw_bigfile_datastream::LibRaw_bigfile_datastream(const wchar_t *fname)
    : filename(), wfilename(fname)
{
  if (wfilename.size() > 0)
  {
    struct _stati64 st;
    if (!_wstati64(wfilename.c_str(), &st))
      _fsize = st.st_size;
#ifndef WIN32SECURECALLS
    f = _wfopen(wfilename.c_str(), L"rb");
#else
    if (_wfopen_s(&f, fname, L"rb"))
      f = 0;
#endif
  }
  else
  {
    wfilename = std::wstring();
    f = 0;
  }
}