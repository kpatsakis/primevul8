MagickExport MagickCLDevice *GetOpenCLDevices(size_t *length,
  ExceptionInfo *magick_unused(exception))
{
  magick_unreferenced(exception);
  if (length != (size_t *) NULL)
    *length=0;
  return((MagickCLDevice *) NULL);
}