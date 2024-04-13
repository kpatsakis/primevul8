MagickExport MagickBooleanType BlobToFile(char *filename,const void *blob,
  const size_t length,ExceptionInfo *exception)
{
  int
    file;

  register size_t
    i;

  ssize_t
    count;

  assert(filename != (const char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",filename);
  assert(blob != (const void *) NULL);
  if (*filename == '\0')
    file=AcquireUniqueFileResource(filename);
  else
    file=open_utf8(filename,O_RDWR | O_CREAT | O_EXCL | O_BINARY,S_MODE);
  if (file == -1)
    {
      ThrowFileException(exception,BlobError,"UnableToWriteBlob",filename);
      return(MagickFalse);
    }
  for (i=0; i < length; i+=count)
  {
    count=write(file,(const char *) blob+i,MagickMin(length-i,(size_t)
      SSIZE_MAX));
    if (count <= 0)
      {
        count=0;
        if (errno != EINTR)
          break;
      }
  }
  file=close(file);
  if ((file == -1) || (i < length))
    {
      ThrowFileException(exception,BlobError,"UnableToWriteBlob",filename);
      return(MagickFalse);
    }
  return(MagickTrue);
}