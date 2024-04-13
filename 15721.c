static inline void ReadTIM2ImageHeader(Image *image,TIM2ImageHeader *header)
{
  header->total_size=ReadBlobLSBLong(image);
  header->clut_size=ReadBlobLSBLong(image);
  header->image_size=ReadBlobLSBLong(image);
  header->header_size=ReadBlobLSBShort(image);

  header->clut_color_count=ReadBlobLSBShort(image);
  header->img_format=(unsigned char) ReadBlobByte(image);
  header->mipmap_count=(unsigned char) ReadBlobByte(image);
  header->clut_type=(unsigned char) ReadBlobByte(image);
  header->bpp_type=(unsigned char) ReadBlobByte(image);

  header->width=ReadBlobLSBShort(image);
  header->height=ReadBlobLSBShort(image);

  header->GsTex0=ReadBlobMSBLongLong(image);
  header->GsTex1=ReadBlobMSBLongLong(image);
  header->GsRegs=ReadBlobMSBLong(image);
  header->GsTexClut=ReadBlobMSBLong(image);
}