MagickExport const void *ReadBlobStream(Image *image,const size_t length,
  void *data,ssize_t *count)
{
  BlobInfo
    *magick_restrict blob_info;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  assert(count != (ssize_t *) NULL);
  blob_info=image->blob;
  if (blob_info->type != BlobStream)
    {
      assert(data != NULL);
      *count=ReadBlob(image,length,(unsigned char *) data);
      return(data);
    }
  if (blob_info->offset >= (MagickOffsetType) blob_info->length)
    {
      *count=0;
      blob_info->eof=MagickTrue;
      return(data);
    }
  data=blob_info->data+blob_info->offset;
  *count=(ssize_t) MagickMin((MagickOffsetType) length,(MagickOffsetType)
    blob_info->length-blob_info->offset);
  blob_info->offset+=(*count);
  if (*count != (ssize_t) length)
    blob_info->eof=MagickTrue;
  return(data);
}