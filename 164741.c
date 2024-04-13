MagickExport ssize_t WriteBlobByte(Image *image,const unsigned char value)
{
  BlobInfo
    *magick_restrict blob_info;

  ssize_t
    count;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  blob_info=image->blob;
  count=0;
  switch (blob_info->type)
  {
    case StandardStream:
    case FileStream:
    case PipeStream:
    {
      int
        c;

      c=putc((int) value,blob_info->file_info.file);
      if (c == EOF)
        break;
      count++;
      break;
    }
    default:
    {
      count=WriteBlobStream(image,1,&value);
      break;
    }
  }
  return(count);
}