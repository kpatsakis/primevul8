int LibRaw_file_datastream::read(void *ptr, size_t size, size_t nmemb)
{
/* Visual Studio 2008 marks sgetn as insecure, but VS2010 does not. */
#if defined(WIN32SECURECALLS) && (_MSC_VER < 1600)
  LR_STREAM_CHK();
  return int(f->_Sgetn_s(static_cast<char *>(ptr), nmemb * size, nmemb * size) /
             (size > 0 ? size : 1));
#else
  LR_STREAM_CHK();
  return int(f->sgetn(static_cast<char *>(ptr), std::streamsize(nmemb * size)) /
             (size > 0 ? size : 1));
#endif
}