ModuleExport size_t RegisterSUNImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("SUN","RAS","SUN Rasterfile");
  entry->decoder=(DecodeImageHandler *) ReadSUNImage;
  entry->encoder=(EncodeImageHandler *) WriteSUNImage;
  entry->magick=(IsImageFormatHandler *) IsSUN;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("SUN","SUN","SUN Rasterfile");
  entry->decoder=(DecodeImageHandler *) ReadSUNImage;
  entry->encoder=(EncodeImageHandler *) WriteSUNImage;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}