MagickExport int ReadBlobByte(Image *image)
{
  BlobInfo
    *magick_restrict blob_info;

  register const unsigned char
    *p;

  unsigned char
    buffer[1];

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->blob != (BlobInfo *) NULL);
  assert(image->blob->type != UndefinedStream);
  blob_info=image->blob;
  switch (blob_info->type)
  {
    case StandardStream:
    case FileStream:
    case PipeStream:
    {
      int
        c;

      p=(const unsigned char *) buffer;
      c=getc(blob_info->file_info.file);
      if (c == EOF)
        return(EOF);
      *buffer=(unsigned char) c;
      break;
    }
    default:
    {
      ssize_t
        count;

      p=(const unsigned char *) ReadBlobStream(image,1,buffer,&count);
      if (count != 1)
        return(EOF);
      break;
    }
  }
  return((int) (*p));
}