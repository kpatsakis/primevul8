ModuleExport size_t RegisterTIM2Image(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("TIM2","TM2","PS2 TIM2");
  entry->decoder=(DecodeImageHandler *) ReadTIM2Image;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}