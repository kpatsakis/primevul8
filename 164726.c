MagickExport CustomStreamInfo *AcquireCustomStreamInfo(
  ExceptionInfo *magick_unused(exception))
{
  CustomStreamInfo
    *custom_stream;

  magick_unreferenced(exception);
  custom_stream=(CustomStreamInfo *) AcquireCriticalMemory(
    sizeof(*custom_stream));
  (void) memset(custom_stream,0,sizeof(*custom_stream));
  custom_stream->signature=MagickCoreSignature;
  return(custom_stream);
}