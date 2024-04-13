int LibRaw_bigfile_datastream::scanf_one(const char *fmt, void *val)
{
  LR_BF_CHK();
  return 
#ifndef WIN32SECURECALLS
                   fscanf(f, fmt, val)
#else
                   fscanf_s(f, fmt, val)
#endif
      ;
}