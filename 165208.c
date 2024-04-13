MagickExport const KernelProfileRecord *GetOpenCLKernelProfileRecords(
  const MagickCLDevice magick_unused(device),size_t *length)
{
  magick_unreferenced(device);
  if (length != (size_t *) NULL)
    *length=0;
  return((const KernelProfileRecord *) NULL);
}