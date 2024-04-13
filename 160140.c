int LibRaw_bigfile_datastream::seek(INT64 o, int whence)
{
  LR_BF_CHK();
#if defined(_WIN32)
#ifdef WIN32SECURECALLS
  return _fseeki64(f, o, whence);
#else
  return fseek(f, (long)o, whence);
#endif
#else
  return fseeko(f, o, whence);
#endif
}