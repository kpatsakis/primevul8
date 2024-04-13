static MagickBooleanType DecodeImage(Image *image,const size_t compression,
  unsigned char *pixels,const size_t number_pixels)
{
  int
    byte,
    count;

  register ssize_t
    i,
    x;

  register unsigned char
    *p,
    *q;

  ssize_t
    y;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(pixels != (unsigned char *) NULL);
  (void) memset(pixels,0,number_pixels*sizeof(*pixels));
  byte=0;
  x=0;
  p=pixels;
  q=pixels+number_pixels;
  for (y=0; y < (ssize_t) image->rows; )
  {
    MagickBooleanType
      status;

    if ((p < pixels) || (p > q))
      break;
    count=ReadBlobByte(image);
    if (count == EOF)
      break;
    if (count > 0)
      {
        /*
          Encoded mode.
        */
        count=(int) MagickMin((ssize_t) count,(ssize_t) (q-p));
        byte=ReadBlobByte(image);
        if (byte == EOF)
          break;
        if (compression == BI_RLE8)
          {
            for (i=0; i < (ssize_t) count; i++)
              *p++=(unsigned char) byte;
          }
        else
          {
            for (i=0; i < (ssize_t) count; i++)
              *p++=(unsigned char)
                ((i & 0x01) != 0 ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
          }
        x+=count;
      }
    else
      {
        /*
          Escape mode.
        */
        count=ReadBlobByte(image);
        if (count == EOF)
          break;
        if (count == 0x01)
          return(MagickTrue);
        switch (count)
        {
          case 0x00:
          {
            /*
              End of line.
            */
            x=0;
            y++;
            p=pixels+y*image->columns;
            break;
          }
          case 0x02:
          {
            /*
              Delta mode.
            */
            x+=ReadBlobByte(image);
            y+=ReadBlobByte(image);
            p=pixels+y*image->columns+x;
            break;
          }
          default:
          {
            /*
              Absolute mode.
            */
            count=(int) MagickMin((ssize_t) count,(ssize_t) (q-p));
            if (compression == BI_RLE8)
              for (i=0; i < (ssize_t) count; i++)
              {
                byte=ReadBlobByte(image);
                if (byte == EOF)
                  break;
                *p++=(unsigned char) byte;
              }
            else
              for (i=0; i < (ssize_t) count; i++)
              {
                if ((i & 0x01) == 0)
                  {
                    byte=ReadBlobByte(image);
                    if (byte == EOF)
                      break;
                  }
                *p++=(unsigned char)
                  ((i & 0x01) != 0 ? (byte & 0x0f) : ((byte >> 4) & 0x0f));
              }
            x+=count;
            /*
              Read pad byte.
            */
            if (compression == BI_RLE8)
              {
                if ((count & 0x01) != 0)
                  if (ReadBlobByte(image) == EOF)
                    break;
              }
            else
              if (((count & 0x03) == 1) || ((count & 0x03) == 2))
                if (ReadBlobByte(image) == EOF)
                  break;
            break;
          }
        }
      }
    status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  (void) ReadBlobByte(image);  /* end of line */
  (void) ReadBlobByte(image);
  return(y < (ssize_t) image->rows ? MagickFalse : MagickTrue);
}