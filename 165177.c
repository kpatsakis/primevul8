static inline MagickBooleanType MagickCreateDirectory(const char *path)
{
  int
    status;

#ifdef MAGICKCORE_WINDOWS_SUPPORT
  status=mkdir(path);
#else
  status=mkdir(path, 0777);
#endif
  return(status == 0 ? MagickTrue : MagickFalse);
}