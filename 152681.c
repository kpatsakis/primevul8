ModuleExport size_t RegisterPICTImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("PICT","PCT","Apple Macintosh QuickDraw/PICT");
  entry->decoder=(DecodeImageHandler *) ReadPICTImage;
  entry->encoder=(EncodeImageHandler *) WritePICTImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderSeekableStreamFlag;
  entry->magick=(IsImageFormatHandler *) IsPICT;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("PICT","PICT","Apple Macintosh QuickDraw/PICT");
  entry->decoder=(DecodeImageHandler *) ReadPICTImage;
  entry->encoder=(EncodeImageHandler *) WritePICTImage;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderSeekableStreamFlag;
  entry->magick=(IsImageFormatHandler *) IsPICT;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}