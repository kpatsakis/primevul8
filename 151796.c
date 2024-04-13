ModuleExport size_t RegisterBMPImage(void)
{
  MagickInfo
    *entry;

  entry=AcquireMagickInfo("BMP","BMP","Microsoft Windows bitmap image");
  entry->decoder=(DecodeImageHandler *) ReadBMPImage;
  entry->encoder=(EncodeImageHandler *) WriteBMPImage;
  entry->magick=(IsImageFormatHandler *) IsBMP;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("BMP","BMP2","Microsoft Windows bitmap image (V2)");
  entry->decoder=(DecodeImageHandler *) ReadBMPImage;
  entry->encoder=(EncodeImageHandler *) WriteBMPImage;
  entry->magick=(IsImageFormatHandler *) IsBMP;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  entry=AcquireMagickInfo("BMP","BMP3","Microsoft Windows bitmap image (V3)");
  entry->decoder=(DecodeImageHandler *) ReadBMPImage;
  entry->encoder=(EncodeImageHandler *) WriteBMPImage;
  entry->magick=(IsImageFormatHandler *) IsBMP;
  entry->flags^=CoderAdjoinFlag;
  entry->flags|=CoderDecoderSeekableStreamFlag;
  (void) RegisterMagickInfo(entry);
  return(MagickImageCoderSignature);
}