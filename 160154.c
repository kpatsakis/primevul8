INT64 LibRaw_bigfile_datastream::tell()
{
  LR_BF_CHK();
#if defined(_WIN32)
#ifdef WIN32SECURECALLS
  return _ftelli64(f);
#else
  return ftell(f);
#endif
#else
  return ftello(f);
#endif
}