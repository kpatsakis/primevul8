static Image *ReadPNMImage(const ImageInfo *image_info,ExceptionInfo *exception)
{
#define ThrowPNMException(exception,message) \
{ \
  if (comment_info.comment != (char *) NULL)  \
    comment_info.comment=DestroyString(comment_info.comment); \
  ThrowReaderException((exception),(message)); \
}

  char
    format;

  CommentInfo
    comment_info;

  double
    quantum_scale;

  Image
    *image;

  MagickBooleanType
    status;

  QuantumAny
    max_value;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  size_t
    depth,
    extent,
    packet_size;

  ssize_t
    count,
    row,
    y;

  /*
    Open image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  image=AcquireImage(image_info,exception);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Read PNM image.
  */
  count=ReadBlob(image,1,(unsigned char *) &format);
  do
  {
    /*
      Initialize image structure.
    */
    comment_info.comment=AcquireString(NULL);
    comment_info.extent=MagickPathExtent;
    if ((count != 1) || (format != 'P'))
      ThrowPNMException(CorruptImageError,"ImproperImageHeader");
    max_value=1;
    quantum_type=RGBQuantum;
    quantum_scale=1.0;
    format=(char) ReadBlobByte(image);
    if (format != '7')
      {
        /*
          PBM, PGM, PPM, and PNM.
        */
        image->columns=(size_t) PNMInteger(image,&comment_info,10,exception);
        image->rows=(size_t) PNMInteger(image,&comment_info,10,exception);
        if ((format == 'f') || (format == 'F'))
          {
            char
              scale[MagickPathExtent];

            if (ReadBlobString(image,scale) != (char *) NULL)
              quantum_scale=StringToDouble(scale,(char **) NULL);
          }
        else
          {
            if ((format == '1') || (format == '4'))
              max_value=1;  /* bitmap */
            else
              max_value=(QuantumAny) PNMInteger(image,&comment_info,10,
                exception);
          }
      }
    else
      {
        char
          keyword[MagickPathExtent],
          value[MagickPathExtent];

        int
          c;

        register char
          *p;

        /*
          PAM.
        */
        for (c=ReadBlobByte(image); c != EOF; c=ReadBlobByte(image))
        {
          while (isspace((int) ((unsigned char) c)) != 0)
            c=ReadBlobByte(image);
          if (c == '#')
            {
              /*
                Comment.
              */
              c=PNMComment(image,&comment_info,exception);
              c=ReadBlobByte(image);
              while (isspace((int) ((unsigned char) c)) != 0)
                c=ReadBlobByte(image);
            }
          p=keyword;
          do
          {
            if ((size_t) (p-keyword) < (MagickPathExtent-1))
              *p++=c;
            c=ReadBlobByte(image);
          } while (isalnum(c));
          *p='\0';
          if (LocaleCompare(keyword,"endhdr") == 0)
            break;
          while (isspace((int) ((unsigned char) c)) != 0)
            c=ReadBlobByte(image);
          p=value;
          while (isalnum(c) || (c == '_'))
          {
            if ((size_t) (p-value) < (MagickPathExtent-1))
              *p++=c;
            c=ReadBlobByte(image);
          }
          *p='\0';
          /*
            Assign a value to the specified keyword.
          */
          if (LocaleCompare(keyword,"depth") == 0)
            packet_size=StringToUnsignedLong(value);
          (void) packet_size;
          if (LocaleCompare(keyword,"height") == 0)
            image->rows=StringToUnsignedLong(value);
          if (LocaleCompare(keyword,"maxval") == 0)
            max_value=StringToUnsignedLong(value);
          if (LocaleCompare(keyword,"TUPLTYPE") == 0)
            {
              if (LocaleCompare(value,"BLACKANDWHITE") == 0)
                {
                  (void) SetImageColorspace(image,GRAYColorspace,exception);
                  quantum_type=GrayQuantum;
                }
              if (LocaleCompare(value,"BLACKANDWHITE_ALPHA") == 0)
                {
                  (void) SetImageColorspace(image,GRAYColorspace,exception);
                  image->alpha_trait=BlendPixelTrait;
                  quantum_type=GrayAlphaQuantum;
                }
              if (LocaleCompare(value,"GRAYSCALE") == 0)
                {
                  quantum_type=GrayQuantum;
                  (void) SetImageColorspace(image,GRAYColorspace,exception);
                }
              if (LocaleCompare(value,"GRAYSCALE_ALPHA") == 0)
                {
                  (void) SetImageColorspace(image,GRAYColorspace,exception);
                  image->alpha_trait=BlendPixelTrait;
                  quantum_type=GrayAlphaQuantum;
                }
              if (LocaleCompare(value,"RGB_ALPHA") == 0)
                {
                  image->alpha_trait=BlendPixelTrait;
                  quantum_type=RGBAQuantum;
                }
              if (LocaleCompare(value,"CMYK") == 0)
                {
                  (void) SetImageColorspace(image,CMYKColorspace,exception);
                  quantum_type=CMYKQuantum;
                }
              if (LocaleCompare(value,"CMYK_ALPHA") == 0)
                {
                  (void) SetImageColorspace(image,CMYKColorspace,exception);
                  image->alpha_trait=BlendPixelTrait;
                  quantum_type=CMYKAQuantum;
                }
            }
          if (LocaleCompare(keyword,"width") == 0)
            image->columns=StringToUnsignedLong(value);
        }
      }
    if ((image->columns == 0) || (image->rows == 0))
      ThrowPNMException(CorruptImageError,"NegativeOrZeroImageSize");
    if ((max_value == 0) || (max_value > 4294967295UL))
      ThrowPNMException(CorruptImageError,"ImproperImageHeader");
    for (depth=1; GetQuantumRange(depth) < max_value; depth++) ;
    image->depth=depth;
    if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if ((MagickSizeType) (image->columns*image->rows/8) > GetBlobSize(image))
      ThrowPNMException(CorruptImageError,"InsufficientImageDataInFile");
    status=SetImageExtent(image,image->columns,image->rows,exception);
    if (status == MagickFalse)
      {
        comment_info.comment=DestroyString(comment_info.comment); \
        return(DestroyImageList(image));
      }
    (void) ResetImagePixels(image,exception);
    /*
      Convert PNM pixels to runextent-encoded MIFF packets.
    */
    row=0;
    y=0;
    switch (format)
    {
      case '1':
      {
        /*
          Convert PBM image to pixel packets.
        */
        (void) SetImageColorspace(image,GRAYColorspace,exception);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register ssize_t
            x;

          register Quantum
            *magick_restrict q;

          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelGray(image,PNMInteger(image,&comment_info,2,exception) ==
              0 ? QuantumRange : 0,q);
            if (EOFBlob(image) != MagickFalse)
              break;
            q+=GetPixelChannels(image);
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
          if (EOFBlob(image) != MagickFalse)
            break;
        }
        image->type=BilevelType;
        break;
      }
      case '2':
      {
        Quantum
          intensity;

        /*
          Convert PGM image to pixel packets.
        */
        (void) SetImageColorspace(image,GRAYColorspace,exception);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register ssize_t
            x;

          register Quantum
            *magick_restrict q;

          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            intensity=ScaleAnyToQuantum(PNMInteger(image,&comment_info,10,
              exception),max_value);
            if (EOFBlob(image) != MagickFalse)
              break;
            SetPixelGray(image,intensity,q);
            q+=GetPixelChannels(image);
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
          if (EOFBlob(image) != MagickFalse)
            break;
        }
        image->type=GrayscaleType;
        break;
      }
      case '3':
      {
        /*
          Convert PNM image to pixel packets.
        */
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          register ssize_t
            x;

          register Quantum
            *magick_restrict q;

          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            Quantum
              pixel;

            pixel=ScaleAnyToQuantum(PNMInteger(image,&comment_info,10,
              exception),max_value);
            if (EOFBlob(image) != MagickFalse)
              break;
            SetPixelRed(image,pixel,q);
            pixel=ScaleAnyToQuantum(PNMInteger(image,&comment_info,10,
              exception),max_value);
            SetPixelGreen(image,pixel,q);
            pixel=ScaleAnyToQuantum(PNMInteger(image,&comment_info,10,
              exception),max_value);
            SetPixelBlue(image,pixel,q);
            q+=GetPixelChannels(image);
          }
          if (SyncAuthenticPixels(image,exception) == MagickFalse)
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
          if (EOFBlob(image) != MagickFalse)
            break;
        }
        break;
      }
      case '4':
      {
        /*
          Convert PBM raw image to pixel packets.
        */
        (void) SetImageColorspace(image,GRAYColorspace,exception);
        quantum_type=GrayQuantum;
        if (image->storage_class == PseudoClass)
          quantum_type=IndexQuantum;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        SetQuantumMinIsWhite(quantum_info,MagickTrue);
        extent=GetQuantumExtent(image,quantum_info,quantum_type);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const unsigned char
            *pixels;

          MagickBooleanType
            sync;

          register Quantum
            *magick_restrict q;

          ssize_t
            offset;

          size_t
            length;

          pixels=(unsigned char *) ReadBlobStream(image,extent,
            GetQuantumPixels(quantum_info),&count);
          if (count != (ssize_t) extent)
            break;
          if ((image->progress_monitor != (MagickProgressMonitor) NULL) &&
              (image->previous == (Image *) NULL))
            {
              MagickBooleanType
                proceed;

              proceed=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                row,image->rows);
              if (proceed == MagickFalse)
                break;
            }
          offset=row++;
          q=QueueAuthenticPixels(image,0,offset,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          length=ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          if (length != extent)
            break;
          sync=SyncAuthenticPixels(image,exception);
          if (sync == MagickFalse)
            break;
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        SetQuantumImageType(image,quantum_type);
        break;
      }
      case '5':
      {
        /*
          Convert PGM raw image to pixel packets.
        */
        (void) SetImageColorspace(image,GRAYColorspace,exception);
        quantum_type=GrayQuantum;
        extent=(image->depth <= 8 ? 1 : image->depth <= 16 ? 2 : 4)*
          image->columns;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const unsigned char
            *pixels;

          MagickBooleanType
            sync;

          register const unsigned char
            *magick_restrict p;

          register ssize_t
            x;

          register Quantum
            *magick_restrict q;

          ssize_t
            offset;

          pixels=(unsigned char *) ReadBlobStream(image,extent,
            GetQuantumPixels(quantum_info),&count);
          if (count != (ssize_t) extent)
            break;
          if ((image->progress_monitor != (MagickProgressMonitor) NULL) &&
              (image->previous == (Image *) NULL))
            {
              MagickBooleanType
                proceed;

              proceed=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                row,image->rows);
              if (proceed == MagickFalse)
                break;
            }
          offset=row++;
          q=QueueAuthenticPixels(image,0,offset,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          p=pixels;
          switch (image->depth)
          {
            case 8:
            case 16:
            case 32:
            {
              (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
                quantum_type,pixels,exception);
              break;
            }
            default:
            {
              if (image->depth <= 8)
                {
                  unsigned char
                    pixel;

                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    p=PushCharPixel(p,&pixel);
                    SetPixelGray(image,ScaleAnyToQuantum(pixel,max_value),q);
                    q+=GetPixelChannels(image);
                  }
                }
              else if (image->depth <= 16)
                {
                  unsigned short
                    pixel;

                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    p=PushShortPixel(MSBEndian,p,&pixel);
                    SetPixelGray(image,ScaleAnyToQuantum(pixel,max_value),q);
                    q+=GetPixelChannels(image);
                  }
                }
              else
                {
                  unsigned int
                    pixel;

                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    p=PushLongPixel(MSBEndian,p,&pixel);
                    SetPixelGray(image,ScaleAnyToQuantum(pixel,max_value),q);
                    q+=GetPixelChannels(image);
                  }
                }
              break;
            }
          }
          sync=SyncAuthenticPixels(image,exception);
          if (sync == MagickFalse)
            break;
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        SetQuantumImageType(image,quantum_type);
        break;
      }
      case '6':
      {
        /*
          Convert PNM raster image to pixel packets.
        */
        quantum_type=RGBQuantum;
        extent=3*(image->depth <= 8 ? 1 : image->depth <= 16 ? 2 : 4)*
          image->columns;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        (void) SetQuantumEndian(image,quantum_info,MSBEndian);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const unsigned char
            *pixels;

          MagickBooleanType
            sync;

          register const unsigned char
            *magick_restrict p;

          register ssize_t
            x;

          register Quantum
            *magick_restrict q;

          ssize_t
            offset;

          pixels=(unsigned char *) ReadBlobStream(image,extent,
            GetQuantumPixels(quantum_info),&count);
          if (count != (ssize_t) extent)
            break;
          if ((image->progress_monitor != (MagickProgressMonitor) NULL) &&
              (image->previous == (Image *) NULL))
            {
              MagickBooleanType
                proceed;

              proceed=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                row,image->rows);
              if (proceed == MagickFalse)
                break;
            }
          offset=row++;
          q=QueueAuthenticPixels(image,0,offset,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          p=pixels;
          switch (image->depth)
          {
            case 8:
            {
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                SetPixelRed(image,ScaleCharToQuantum(*p++),q);
                SetPixelGreen(image,ScaleCharToQuantum(*p++),q);
                SetPixelBlue(image,ScaleCharToQuantum(*p++),q);
                SetPixelAlpha(image,OpaqueAlpha,q);
                q+=GetPixelChannels(image);
              }
              break;
            }
            case 16:
            {
              unsigned short
                pixel;

              for (x=0; x < (ssize_t) image->columns; x++)
              {
                p=PushShortPixel(MSBEndian,p,&pixel);
                SetPixelRed(image,ScaleShortToQuantum(pixel),q);
                p=PushShortPixel(MSBEndian,p,&pixel);
                SetPixelGreen(image,ScaleShortToQuantum(pixel),q);
                p=PushShortPixel(MSBEndian,p,&pixel);
                SetPixelBlue(image,ScaleShortToQuantum(pixel),q);
                SetPixelAlpha(image,OpaqueAlpha,q);
                q+=GetPixelChannels(image);
              }
              break;
            }
            case 32:
            {
              unsigned int
                pixel;

              for (x=0; x < (ssize_t) image->columns; x++)
              {
                p=PushLongPixel(MSBEndian,p,&pixel);
                SetPixelRed(image,ScaleLongToQuantum(pixel),q);
                p=PushLongPixel(MSBEndian,p,&pixel);
                SetPixelGreen(image,ScaleLongToQuantum(pixel),q);
                p=PushLongPixel(MSBEndian,p,&pixel);
                SetPixelBlue(image,ScaleLongToQuantum(pixel),q);
                SetPixelAlpha(image,OpaqueAlpha,q);
                q+=GetPixelChannels(image);
              }
              break;
            }
            default:
            {
              if (image->depth <= 8)
                {
                  unsigned char
                    pixel;

                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    p=PushCharPixel(p,&pixel);
                    SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                    p=PushCharPixel(p,&pixel);
                    SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),q);
                    p=PushCharPixel(p,&pixel);
                    SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),q);
                    SetPixelAlpha(image,OpaqueAlpha,q);
                    q+=GetPixelChannels(image);
                  }
                }
              else if (image->depth <= 16)
                {
                  unsigned short
                    pixel;

                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    p=PushShortPixel(MSBEndian,p,&pixel);
                    SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                    p=PushShortPixel(MSBEndian,p,&pixel);
                    SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),q);
                    p=PushShortPixel(MSBEndian,p,&pixel);
                    SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),q);
                    SetPixelAlpha(image,OpaqueAlpha,q);
                    q+=GetPixelChannels(image);
                  }
                }
              else
                {
                  unsigned int
                    pixel;

                  for (x=0; x < (ssize_t) image->columns; x++)
                  {
                    p=PushLongPixel(MSBEndian,p,&pixel);
                    SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                    p=PushLongPixel(MSBEndian,p,&pixel);
                    SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),q);
                    p=PushLongPixel(MSBEndian,p,&pixel);
                    SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),q);
                    SetPixelAlpha(image,OpaqueAlpha,q);
                    q+=GetPixelChannels(image);
                  }
                }
              break;
            }
          }
          sync=SyncAuthenticPixels(image,exception);
          if (sync == MagickFalse)
            break;
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        break;
      }
      case '7':
      {
        size_t
          channels;

        /*
          Convert PAM raster image to pixel packets.
        */
        switch (quantum_type)
        {
          case GrayQuantum:
          case GrayAlphaQuantum:
          {
            channels=1;
            break;
          }
          case CMYKQuantum:
          case CMYKAQuantum:
          {
            channels=4;
            break;
          }
          default:
          {
            channels=3;
            break;
          }
        }
        if (image->alpha_trait != UndefinedPixelTrait)
          channels++;
        extent=channels*(image->depth <= 8 ? 1 : image->depth <= 16 ? 2 : 4)*
          image->columns;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const unsigned char
            *pixels;

          MagickBooleanType
            sync;

          register const unsigned char
            *magick_restrict p;

          register ssize_t
            x;

          register Quantum
            *magick_restrict q;

          ssize_t
            offset;

          pixels=(unsigned char *) ReadBlobStream(image,extent,
            GetQuantumPixels(quantum_info),&count);
          if (count != (ssize_t) extent)
            break;
          if ((image->progress_monitor != (MagickProgressMonitor) NULL) &&
              (image->previous == (Image *) NULL))
            {
              MagickBooleanType
                proceed;

              proceed=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                row,image->rows);
              if (proceed == MagickFalse)
                break;
            }
          offset=row++;
          q=QueueAuthenticPixels(image,0,offset,image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          p=pixels;
          switch (image->depth)
          {
            case 8:
            case 16:
            case 32:
            {
              (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
                quantum_type,pixels,exception);
              break;
            }
            default:
            {
              switch (quantum_type)
              {
                case GrayQuantum:
                case GrayAlphaQuantum:
                {
                  if (image->depth <= 8)
                    {
                      unsigned char
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushCharPixel(p,&pixel);
                        SetPixelGray(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushCharPixel(p,&pixel);
                            if (image->depth != 1)
                              SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                                max_value),q);
                            else
                              SetPixelAlpha(image,QuantumRange-
                                ScaleAnyToQuantum(pixel,max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  else if (image->depth <= 16)
                    {
                      unsigned short
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelGray(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushShortPixel(MSBEndian,p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  else
                    {
                      unsigned int
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelGray(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushLongPixel(MSBEndian,p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  break;
                }
                case CMYKQuantum:
                case CMYKAQuantum:
                {
                  if (image->depth <= 8)
                    {
                      unsigned char
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushCharPixel(p,&pixel);
                        SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                        p=PushCharPixel(p,&pixel);
                        SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushCharPixel(p,&pixel);
                        SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushCharPixel(p,&pixel);
                        SetPixelBlack(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushCharPixel(p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  else if (image->depth <= 16)
                    {
                      unsigned short
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelBlack(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushShortPixel(MSBEndian,p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  else
                    {
                      unsigned int
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelBlack(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushLongPixel(MSBEndian,p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                    }
                  }
                  break;
                }
                default:
                {
                  if (image->depth <= 8)
                    {
                      unsigned char
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushCharPixel(p,&pixel);
                        SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                        p=PushCharPixel(p,&pixel);
                        SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushCharPixel(p,&pixel);
                        SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushCharPixel(p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  else if (image->depth <= 16)
                    {
                      unsigned short
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),q);
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushShortPixel(MSBEndian,p,&pixel);
                        SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushShortPixel(MSBEndian,p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  else
                    {
                      unsigned int
                        pixel;

                      for (x=0; x < (ssize_t) image->columns; x++)
                      {
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelRed(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelGreen(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        p=PushLongPixel(MSBEndian,p,&pixel);
                        SetPixelBlue(image,ScaleAnyToQuantum(pixel,max_value),
                          q);
                        SetPixelAlpha(image,OpaqueAlpha,q);
                        if (image->alpha_trait != UndefinedPixelTrait)
                          {
                            p=PushLongPixel(MSBEndian,p,&pixel);
                            SetPixelAlpha(image,ScaleAnyToQuantum(pixel,
                              max_value),q);
                          }
                        q+=GetPixelChannels(image);
                      }
                    }
                  break;
                }
              }
            }
          }
          sync=SyncAuthenticPixels(image,exception);
          if (sync == MagickFalse)
            break;
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        SetQuantumImageType(image,quantum_type);
        break;
      }
      case 'F':
      case 'f':
      {
        /*
          Convert PFM raster image to pixel packets.
        */
        if (format == 'f')
          (void) SetImageColorspace(image,GRAYColorspace,exception);
        quantum_type=format == 'f' ? GrayQuantum : RGBQuantum;
        image->endian=quantum_scale < 0.0 ? LSBEndian : MSBEndian;
        image->depth=32;
        quantum_info=AcquireQuantumInfo(image_info,image);
        if (quantum_info == (QuantumInfo *) NULL)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        status=SetQuantumDepth(image,quantum_info,32);
        if (status == MagickFalse)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        status=SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
        if (status == MagickFalse)
          ThrowPNMException(ResourceLimitError,"MemoryAllocationFailed");
        SetQuantumScale(quantum_info,(double) QuantumRange*fabs(quantum_scale));
        extent=GetQuantumExtent(image,quantum_info,quantum_type);
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          const unsigned char
            *pixels;

          MagickBooleanType
            sync;

          register Quantum
            *magick_restrict q;

          ssize_t
            offset;

          size_t
            length;

          pixels=(unsigned char *) ReadBlobStream(image,extent,
            GetQuantumPixels(quantum_info),&count);
          if (count != (ssize_t) extent)
            break;
          if ((image->progress_monitor != (MagickProgressMonitor) NULL) &&
              (image->previous == (Image *) NULL))
            {
              MagickBooleanType
                proceed;

              proceed=SetImageProgress(image,LoadImageTag,(MagickOffsetType)
                row,image->rows);
              if (proceed == MagickFalse)
                break;
            }
          offset=row++;
          q=QueueAuthenticPixels(image,0,(ssize_t) (image->rows-offset-1),
            image->columns,1,exception);
          if (q == (Quantum *) NULL)
            break;
          length=ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          if (length != extent)
            break;
          sync=SyncAuthenticPixels(image,exception);
          if (sync == MagickFalse)
            break;
        }
        quantum_info=DestroyQuantumInfo(quantum_info);
        SetQuantumImageType(image,quantum_type);
        break;
      }
      default:
        ThrowPNMException(CorruptImageError,"ImproperImageHeader");
    }
    if (*comment_info.comment != '\0')
      (void) SetImageProperty(image,"comment",comment_info.comment,exception);
    comment_info.comment=DestroyString(comment_info.comment);
    if (y < (ssize_t) image->rows)
      ThrowPNMException(CorruptImageError,"UnableToReadImageData");
    if (EOFBlob(image) != MagickFalse)
      {
        (void) ThrowMagickException(exception,GetMagickModule(),
          CorruptImageError,"UnexpectedEndOfFile","`%s'",image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    if ((format == '1') || (format == '2') || (format == '3'))
      do
      {
        /*
          Skip to end of line.
        */
        count=ReadBlob(image,1,(unsigned char *) &format);
        if (count != 1)
          break;
        if (format == 'P')
          break;
      } while (format != '\n');
    count=ReadBlob(image,1,(unsigned char *) &format);
    if ((count == 1) && (format == 'P'))
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image,exception);
        if (GetNextImageInList(image) == (Image *) NULL)
          {
            status=MagickFalse;
            break;
          }
        image=SyncNextImageInList(image);
        status=SetImageProgress(image,LoadImagesTag,TellBlob(image),
          GetBlobSize(image));
        if (status == MagickFalse)
          break;
      }
  } while ((count == 1) && (format == 'P'));
  (void) CloseBlob(image);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  return(GetFirstImageInList(image));
}