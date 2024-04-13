ModuleExport size_t RegisterSGIImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("SGI");
  entry->decoder=(DecodeImageHandler *) ReadSGIImage;
  entry->encoder=(EncodeImageHandler *) WriteSGIImage;
  entry->magick=(IsImageFormatHandler *) IsSGI;
  entry->description=ConstantString("Irix RGB image");
  entry->module=ConstantString("SGI");
  entry->seekable_stream=MagickTrue;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}