ModuleExport size_t RegisterCUTImage(void)
{
  MagickInfo
    *entry;

  entry=SetMagickInfo("CUT");
  entry->decoder=(DecodeImageHandler *) ReadCUTImage;
  entry->seekable_stream=MagickTrue;
  entry->description=ConstantString("DR Halo");
  entry->module=ConstantString("CUT");
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}