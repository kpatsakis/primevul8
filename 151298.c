static Image *ReadMIFFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define BZipMaxExtent(x)  ((x)+((x)/100)+600)
#define LZMAMaxExtent(x)  ((x)+((x)/3)+128)
#define ThrowMIFFException(exception,message) \
{ \
  if (quantum_info != (QuantumInfo *) NULL) \
    quantum_info=DestroyQuantumInfo(quantum_info); \
  if (compress_pixels != (unsigned char *) NULL) \
    compress_pixels=(unsigned char *) RelinquishMagickMemory(compress_pixels); \
  ThrowReaderException((exception),(message)); \
}
#define ZipMaxExtent(x)  ((x)+(((x)+7) >> 3)+(((x)+63) >> 6)+11)

#if defined(MAGICKCORE_BZLIB_DELEGATE)
  bz_stream
    bzip_info;
#endif

  char
    id[MaxTextExtent],
    keyword[MaxTextExtent],
    *options;

  const unsigned char
    *p;

  double
    version;

  GeometryInfo
    geometry_info;

  Image
    *image;

  IndexPacket
    index;

  int
    c;

  LinkedListInfo
    *profiles;

#if defined(MAGICKCORE_LZMA_DELEGATE)
  lzma_stream
    initialize_lzma = LZMA_STREAM_INIT,
    lzma_info;

  lzma_allocator
    allocator;
#endif

  MagickBooleanType
    status;

  MagickStatusType
    flags;

  PixelPacket
    pixel;

  QuantumFormatType
    quantum_format;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register ssize_t
    i;

  size_t
    compress_extent,
    length,
    packet_size;

  ssize_t
    count;

  unsigned char
    *compress_pixels,
    *pixels;

  size_t
    colors;

  ssize_t
    y;

#if defined(MAGICKCORE_ZLIB_DELEGATE)
  z_stream
    zip_info;
#endif

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
  image=AcquireImage(image_info);
  status=OpenBlob(image_info,image,ReadBinaryBlobMode,exception);
  if (status == MagickFalse)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  /*
    Decode image header;  header terminates one character beyond a ':'.
  */
  c=ReadBlobByte(image);
  if (c == EOF)
    ThrowReaderException(CorruptImageError,"ImproperImageHeader");
  *id='\0';
  compress_pixels=(unsigned char *) NULL;
  quantum_info=(QuantumInfo *) NULL;
  (void) memset(keyword,0,sizeof(keyword));
  version=0.0;
  (void) version;
  do
  {
    /*
      Decode image header;  header terminates one character beyond a ':'.
    */
    SetGeometryInfo(&geometry_info);
    length=MaxTextExtent;
    options=AcquireString((char *) NULL);
    quantum_format=UndefinedQuantumFormat;
    profiles=(LinkedListInfo *) NULL;
    colors=0;
    image->depth=8UL;
    image->compression=NoCompression;
    while ((isgraph(c) != MagickFalse) && (c != (int) ':'))
    {
      register char
        *p;

      if (c == (int) '{')
        {
          char
            *comment;

          /*
            Read comment-- any text between { }.
          */
          length=MaxTextExtent;
          comment=AcquireString((char *) NULL);
          for (p=comment; comment != (char *) NULL; p++)
          {
            c=ReadBlobByte(image);
            if (c == (int) '\\')
              c=ReadBlobByte(image);
            else
              if ((c == EOF) || (c == (int) '}'))
                break;
            if ((size_t) (p-comment+1) >= length)
              {
                *p='\0';
                length<<=1;
                comment=(char *) ResizeQuantumMemory(comment,length+
                  MaxTextExtent,sizeof(*comment));
                if (comment == (char *) NULL)
                  break;
                p=comment+strlen(comment);
              }
            *p=(char) c;
          }
          if (comment == (char *) NULL)
            {
              options=DestroyString(options);
              ThrowMIFFException(ResourceLimitError,"MemoryAllocationFailed");
            }
          *p='\0';
          (void) SetImageProperty(image,"comment",comment);
          comment=DestroyString(comment);
          c=ReadBlobByte(image);
        }
      else
        if (isalnum(c) != MagickFalse)
          {
            /*
              Get the keyword.
            */
            length=MaxTextExtent-1;
            p=keyword;
            do
            {
              if (c == (int) '=')
                break;
              if ((size_t) (p-keyword) < (MaxTextExtent-1))
                *p++=(char) c;
              c=ReadBlobByte(image);
            } while (c != EOF);
            *p='\0';
            p=options;
            while ((isspace((int) ((unsigned char) c)) != 0) && (c != EOF))
              c=ReadBlobByte(image);
            if (c == (int) '=')
              {
                /*
                  Get the keyword value.
                */
                c=ReadBlobByte(image);
                while ((c != (int) '}') && (c != EOF))
                {
                  if ((size_t) (p-options+1) >= length)
                    {
                      *p='\0';
                      length<<=1;
                      options=(char *) ResizeQuantumMemory(options,length+
                        MaxTextExtent,sizeof(*options));
                      if (options == (char *) NULL)
                        break;
                      p=options+strlen(options);
                    }
                  *p++=(char) c;
                  c=ReadBlobByte(image);
                  if (c == '\\')
                    {
                      c=ReadBlobByte(image);
                      if (c == (int) '}')
                        {
                          *p++=(char) c;
                          c=ReadBlobByte(image);
                        }
                    }
                  if (*options != '{')
                    if (isspace((int) ((unsigned char) c)) != 0)
                      break;
                }
                if (options == (char *) NULL)
                  ThrowMIFFException(ResourceLimitError,
                    "MemoryAllocationFailed");
              }
            *p='\0';
            if (*options == '{')
              (void) CopyMagickString(options,options+1,strlen(options));
            /*
              Assign a value to the specified keyword.
            */
            switch (*keyword)
            {
              case 'b':
              case 'B':
              {
                if (LocaleCompare(keyword,"background-color") == 0)
                  {
                    (void) QueryColorDatabase(options,&image->background_color,
                      exception);
                    break;
                  }
                if (LocaleCompare(keyword,"blue-primary") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.blue_primary.x=geometry_info.rho;
                    image->chromaticity.blue_primary.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.blue_primary.y=
                        image->chromaticity.blue_primary.x;
                    break;
                  }
                if (LocaleCompare(keyword,"border-color") == 0)
                  {
                    (void) QueryColorDatabase(options,&image->border_color,
                      exception);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'c':
              case 'C':
              {
                if (LocaleCompare(keyword,"class") == 0)
                  {
                    ssize_t
                      storage_class;

                    storage_class=ParseCommandOption(MagickClassOptions,
                      MagickFalse,options);
                    if (storage_class < 0)
                      break;
                    image->storage_class=(ClassType) storage_class;
                    break;
                  }
                if (LocaleCompare(keyword,"colors") == 0)
                  {
                    colors=StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"colorspace") == 0)
                  {
                    ssize_t
                      colorspace;

                    colorspace=ParseCommandOption(MagickColorspaceOptions,
                      MagickFalse,options);
                    if (colorspace < 0)
                      break;
                    image->colorspace=(ColorspaceType) colorspace;
                    break;
                  }
                if (LocaleCompare(keyword,"compression") == 0)
                  {
                    ssize_t
                      compression;

                    compression=ParseCommandOption(MagickCompressOptions,
                      MagickFalse,options);
                    if (compression < 0)
                      break;
                    image->compression=(CompressionType) compression;
                    break;
                  }
                if (LocaleCompare(keyword,"columns") == 0)
                  {
                    image->columns=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'd':
              case 'D':
              {
                if (LocaleCompare(keyword,"delay") == 0)
                  {
                    image->delay=StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"depth") == 0)
                  {
                    image->depth=StringToUnsignedLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"dispose") == 0)
                  {
                    ssize_t
                      dispose;

                    dispose=ParseCommandOption(MagickDisposeOptions,MagickFalse,
                      options);
                    if (dispose < 0)
                      break;
                    image->dispose=(DisposeType) dispose;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'e':
              case 'E':
              {
                if (LocaleCompare(keyword,"endian") == 0)
                  {
                    ssize_t
                      endian;

                    endian=ParseCommandOption(MagickEndianOptions,MagickFalse,
                      options);
                    if (endian < 0)
                      break;
                    image->endian=(EndianType) endian;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'g':
              case 'G':
              {
                if (LocaleCompare(keyword,"gamma") == 0)
                  {
                    image->gamma=StringToDouble(options,(char **) NULL);
                    break;
                  }
                if (LocaleCompare(keyword,"gravity") == 0)
                  {
                    ssize_t
                      gravity;

                    gravity=ParseCommandOption(MagickGravityOptions,MagickFalse,
                      options);
                    if (gravity < 0)
                      break;
                    image->gravity=(GravityType) gravity;
                    break;
                  }
                if (LocaleCompare(keyword,"green-primary") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.green_primary.x=geometry_info.rho;
                    image->chromaticity.green_primary.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.green_primary.y=
                        image->chromaticity.green_primary.x;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'i':
              case 'I':
              {
                if (LocaleCompare(keyword,"id") == 0)
                  {
                    (void) CopyMagickString(id,options,MaxTextExtent);
                    break;
                  }
                if (LocaleCompare(keyword,"iterations") == 0)
                  {
                    image->iterations=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'm':
              case 'M':
              {
                if (LocaleCompare(keyword,"matte") == 0)
                  {
                    ssize_t
                      matte;

                    matte=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                      options);
                    if (matte < 0)
                      break;
                    image->matte=(MagickBooleanType) matte;
                    break;
                  }
                if (LocaleCompare(keyword,"matte-color") == 0)
                  {
                    (void) QueryColorDatabase(options,&image->matte_color,
                      exception);
                    break;
                  }
                if (LocaleCompare(keyword,"montage") == 0)
                  {
                    (void) CloneString(&image->montage,options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'o':
              case 'O':
              {
                if (LocaleCompare(keyword,"opaque") == 0)
                  {
                    ssize_t
                      matte;

                    matte=ParseCommandOption(MagickBooleanOptions,MagickFalse,
                      options);
                    if (matte < 0)
                      break;
                    image->matte=(MagickBooleanType) matte;
                    break;
                  }
                if (LocaleCompare(keyword,"orientation") == 0)
                  {
                    ssize_t
                      orientation;

                    orientation=ParseCommandOption(MagickOrientationOptions,
                      MagickFalse,options);
                    if (orientation < 0)
                      break;
                    image->orientation=(OrientationType) orientation;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'p':
              case 'P':
              {
                if (LocaleCompare(keyword,"page") == 0)
                  {
                    char
                      *geometry;

                    geometry=GetPageGeometry(options);
                    (void) ParseAbsoluteGeometry(geometry,&image->page);
                    geometry=DestroyString(geometry);
                    break;
                  }
                if (LocaleCompare(keyword,"pixel-intensity") == 0)
                  {
                    ssize_t
                      intensity;

                    intensity=ParseCommandOption(MagickPixelIntensityOptions,
                      MagickFalse,options);
                    if (intensity < 0)
                      break;
                    image->intensity=(PixelIntensityMethod) intensity;
                    break;
                  }
                if (LocaleCompare(keyword,"profile") == 0)
                  {
                    if (profiles == (LinkedListInfo *) NULL)
                      profiles=NewLinkedList(0);
                    (void) AppendValueToLinkedList(profiles,
                      AcquireString(options));
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'q':
              case 'Q':
              {
                if (LocaleCompare(keyword,"quality") == 0)
                  {
                    image->quality=StringToUnsignedLong(options);
                    break;
                  }
                if ((LocaleCompare(keyword,"quantum-format") == 0) ||
                    (LocaleCompare(keyword,"quantum:format") == 0))
                  {
                    ssize_t
                      format;

                    format=ParseCommandOption(MagickQuantumFormatOptions,
                      MagickFalse,options);
                    if (format < 0)
                      break;
                    quantum_format=(QuantumFormatType) format;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'r':
              case 'R':
              {
                if (LocaleCompare(keyword,"red-primary") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.red_primary.x=geometry_info.rho;
                    image->chromaticity.red_primary.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.red_primary.y=
                        image->chromaticity.red_primary.x;
                    break;
                  }
                if (LocaleCompare(keyword,"rendering-intent") == 0)
                  {
                    ssize_t
                      rendering_intent;

                    rendering_intent=ParseCommandOption(MagickIntentOptions,
                      MagickFalse,options);
                    if (rendering_intent < 0)
                      break;
                    image->rendering_intent=(RenderingIntent) rendering_intent;
                    break;
                  }
                if (LocaleCompare(keyword,"resolution") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->x_resolution=geometry_info.rho;
                    image->y_resolution=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->y_resolution=image->x_resolution;
                    break;
                  }
                if (LocaleCompare(keyword,"rows") == 0)
                  {
                    image->rows=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 's':
              case 'S':
              {
                if (LocaleCompare(keyword,"scene") == 0)
                  {
                    image->scene=StringToUnsignedLong(options);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 't':
              case 'T':
              {
                if (LocaleCompare(keyword,"ticks-per-second") == 0)
                  {
                    image->ticks_per_second=(ssize_t) StringToLong(options);
                    break;
                  }
                if (LocaleCompare(keyword,"tile-offset") == 0)
                  {
                    char
                      *geometry;

                    geometry=GetPageGeometry(options);
                    (void) ParseAbsoluteGeometry(geometry,&image->tile_offset);
                    geometry=DestroyString(geometry);
                    break;
                  }
                if (LocaleCompare(keyword,"type") == 0)
                  {
                    ssize_t
                      type;

                    type=ParseCommandOption(MagickTypeOptions,MagickFalse,
                      options);
                    if (type < 0)
                      break;
                    image->type=(ImageType) type;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'u':
              case 'U':
              {
                if (LocaleCompare(keyword,"units") == 0)
                  {
                    ssize_t
                      units;

                    units=ParseCommandOption(MagickResolutionOptions,
                      MagickFalse,options);
                    if (units < 0)
                      break;
                    image->units=(ResolutionType) units;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'v':
              case 'V':
              {
                if (LocaleCompare(keyword,"version") == 0)
                  {
                    version=StringToDouble(options,(char **) NULL);
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              case 'w':
              case 'W':
              {
                if (LocaleCompare(keyword,"white-point") == 0)
                  {
                    flags=ParseGeometry(options,&geometry_info);
                    image->chromaticity.white_point.x=geometry_info.rho;
                    image->chromaticity.white_point.y=geometry_info.sigma;
                    if ((flags & SigmaValue) == 0)
                      image->chromaticity.white_point.y=
                        image->chromaticity.white_point.x;
                    break;
                  }
                (void) SetImageProperty(image,keyword,options);
                break;
              }
              default:
              {
                (void) SetImageProperty(image,keyword,options);
                break;
              }
            }
          }
        else
          c=ReadBlobByte(image);
      while (isspace((int) ((unsigned char) c)) != 0)
        c=ReadBlobByte(image);
    }
    options=DestroyString(options);
    (void) ReadBlobByte(image);
    /*
      Verify that required image information is defined.
    */
    if ((LocaleCompare(id,"ImageMagick") != 0) || (image->depth > 128) ||
        (image->storage_class == UndefinedClass) ||
        (image->compression == UndefinedCompression) ||
        (image->colorspace == UndefinedColorspace) ||
        (image->columns == 0) || (image->rows == 0))
      {
        if (profiles != (LinkedListInfo *) NULL)
          profiles=DestroyLinkedList(profiles,RelinquishMagickMemory);
        if (image->previous == (Image *) NULL)
          ThrowMIFFException(CorruptImageError,"ImproperImageHeader");
        DeleteImageFromList(&image);
        (void) ThrowMagickException(&image->exception,GetMagickModule(),
          CorruptImageError,"ImproperImageHeader","`%s'",image->filename);
        break;
      }
    if (image->montage != (char *) NULL)
      {
        register char
          *p;

        /*
          Image directory.
        */
        length=MaxTextExtent;
        image->directory=AcquireString((char *) NULL);
        p=image->directory;
        do
        {
          *p='\0';
          if ((strlen(image->directory)+MaxTextExtent) >= length)
            {
              /*
                Allocate more memory for the image directory.
              */
              length<<=1;
              image->directory=(char *) ResizeQuantumMemory(image->directory,
                length+MaxTextExtent,sizeof(*image->directory));
              if (image->directory == (char *) NULL)
                ThrowMIFFException(CorruptImageError,"UnableToReadImageData");
              p=image->directory+strlen(image->directory);
            }
          c=ReadBlobByte(image);
          if (c == EOF)
            break;
          *p++=(char) c;
        } while (c != (int) '\0');
      }
    if (profiles != (LinkedListInfo *) NULL)
      {
        const char
          *name;

        StringInfo
          *profile;

        /*
          Read image profile blobs.
        */
        ResetLinkedListIterator(profiles);
        name=(const char *) GetNextValueInLinkedList(profiles);
        while (name != (const char *) NULL)
        {
          ssize_t
            count;

          length=ReadBlobMSBLong(image);
          if ((MagickSizeType) length > GetBlobSize(image))
            break;
          profile=AcquireStringInfo(length);
          if (profile == (StringInfo *) NULL)
            break;
          count=ReadBlob(image,length,GetStringInfoDatum(profile));
          if (count != (ssize_t) length)
            {
              profile=DestroyStringInfo(profile);
              break;
            }
          status=SetImageProfile(image,name,profile);
          profile=DestroyStringInfo(profile);
          if (status == MagickFalse)
            break;
          name=(const char *) GetNextValueInLinkedList(profiles);
        }
        profiles=DestroyLinkedList(profiles,RelinquishMagickMemory);
      }
    image->depth=GetImageQuantumDepth(image,MagickFalse);
    if (image->storage_class == PseudoClass)
      {
        size_t
          packet_size;

        unsigned char
          *colormap;

        /*
          Create image colormap.
        */
        packet_size=(size_t) (3UL*image->depth/8UL);
        if ((MagickSizeType) colors > GetBlobSize(image))
          ThrowMIFFException(CorruptImageError,"InsufficientImageDataInFile");
        if (((MagickSizeType) packet_size*colors) > GetBlobSize(image))
          ThrowMIFFException(CorruptImageError,"InsufficientImageDataInFile");
        status=AcquireImageColormap(image,colors != 0 ? colors : 256);
        if (status == MagickFalse)
          {
            InheritException(exception,&image->exception);
            return(DestroyImageList(image));
          }
        if (colors != 0)
          {
            /*
              Read image colormap from file.
            */
            colormap=(unsigned char *) AcquireQuantumMemory(image->colors,
              packet_size*sizeof(*colormap));
            if (colormap == (unsigned char *) NULL)
              ThrowMIFFException(ResourceLimitError,"MemoryAllocationFailed");
            (void) ReadBlob(image,packet_size*image->colors,colormap);
            p=colormap;
            switch (image->depth)
            {
              default:
                colormap=(unsigned char *) RelinquishMagickMemory(colormap);
                ThrowMIFFException(CorruptImageError,"ImageDepthNotSupported");
              case 8:
              {
                unsigned char
                  pixel;

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  p=PushCharPixel(p,&pixel);
                  image->colormap[i].red=ScaleCharToQuantum(pixel);
                  p=PushCharPixel(p,&pixel);
                  image->colormap[i].green=ScaleCharToQuantum(pixel);
                  p=PushCharPixel(p,&pixel);
                  image->colormap[i].blue=ScaleCharToQuantum(pixel);
                }
                break;
              }
              case 16:
              {
                unsigned short
                  pixel;

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  p=PushShortPixel(MSBEndian,p,&pixel);
                  image->colormap[i].red=ScaleShortToQuantum(pixel);
                  p=PushShortPixel(MSBEndian,p,&pixel);
                  image->colormap[i].green=ScaleShortToQuantum(pixel);
                  p=PushShortPixel(MSBEndian,p,&pixel);
                  image->colormap[i].blue=ScaleShortToQuantum(pixel);
                }
                break;
              }
              case 32:
              {
                unsigned int
                  pixel;

                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  p=PushLongPixel(MSBEndian,p,&pixel);
                  image->colormap[i].red=ScaleLongToQuantum(pixel);
                  p=PushLongPixel(MSBEndian,p,&pixel);
                  image->colormap[i].green=ScaleLongToQuantum(pixel);
                  p=PushLongPixel(MSBEndian,p,&pixel);
                  image->colormap[i].blue=ScaleLongToQuantum(pixel);
                }
                break;
              }
            }
            colormap=(unsigned char *) RelinquishMagickMemory(colormap);
          }
      }
    if ((image_info->ping != MagickFalse) && (image_info->number_scenes != 0))
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    status=ResetImagePixels(image,exception);
    if (status == MagickFalse)
      {
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    /*
      Allocate image pixels.
    */
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowMIFFException(ResourceLimitError,"MemoryAllocationFailed");
    if (quantum_format != UndefinedQuantumFormat)
      {
        status=SetQuantumFormat(image,quantum_info,quantum_format);
        if (status == MagickFalse)
          ThrowMIFFException(ResourceLimitError,"MemoryAllocationFailed");
      }
    packet_size=(size_t) (quantum_info->depth/8);
    if (image->storage_class == DirectClass)
      packet_size=(size_t) (3*quantum_info->depth/8);
    if (IsGrayColorspace(image->colorspace) != MagickFalse)
      packet_size=quantum_info->depth/8;
    if (image->matte != MagickFalse)
      packet_size+=quantum_info->depth/8;
    if (image->colorspace == CMYKColorspace)
      packet_size+=quantum_info->depth/8;
    if (image->compression == RLECompression)
      packet_size++;
    compress_extent=MagickMax(MagickMax(BZipMaxExtent(packet_size*
      image->columns),LZMAMaxExtent(packet_size*image->columns)),
      ZipMaxExtent(packet_size*image->columns));
    compress_pixels=(unsigned char *) AcquireQuantumMemory(compress_extent,
      sizeof(*compress_pixels));
    if (compress_pixels == (unsigned char *) NULL)
      ThrowMIFFException(ResourceLimitError,"MemoryAllocationFailed");
    /*
      Read image pixels.
    */
    quantum_type=RGBQuantum;
    if (image->matte != MagickFalse)
      quantum_type=RGBAQuantum;
    if (image->colorspace == CMYKColorspace)
      {
        quantum_type=CMYKQuantum;
        if (image->matte != MagickFalse)
          quantum_type=CMYKAQuantum;
      }
    if (IsGrayColorspace(image->colorspace) != MagickFalse)
      {
        quantum_type=GrayQuantum;
        if (image->matte != MagickFalse)
          quantum_type=GrayAlphaQuantum;
      }
    if (image->storage_class == PseudoClass)
      {
        quantum_type=IndexQuantum;
        if (image->matte != MagickFalse)
          quantum_type=IndexAlphaQuantum;
      }
    status=MagickTrue;
    (void) memset(&pixel,0,sizeof(pixel));
#if defined(MAGICKCORE_BZLIB_DELEGATE)
    (void) memset(&bzip_info,0,sizeof(bzip_info));
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
    (void) memset(&allocator,0,sizeof(allocator));
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
    (void) memset(&zip_info,0,sizeof(zip_info));
#endif
    switch (image->compression)
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      case BZipCompression:
      {
        int
          code;

        bzip_info.bzalloc=AcquireBZIPMemory;
        bzip_info.bzfree=RelinquishBZIPMemory;
        bzip_info.opaque=(void *) image;
        code=BZ2_bzDecompressInit(&bzip_info,(int) image_info->verbose,
          MagickFalse);
        if (code != BZ_OK)
          status=MagickFalse;
        break;
      }
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
      case LZMACompression:
      {
        int
          code;

        allocator.alloc=AcquireLZMAMemory;
        allocator.free=RelinquishLZMAMemory;
        allocator.opaque=(void *) image;
        lzma_info=initialize_lzma;
        lzma_info.allocator=(&allocator);
        code=lzma_auto_decoder(&lzma_info,-1,0);
        if (code != LZMA_OK)
          status=MagickFalse;
        break;
      }
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      case LZWCompression:
      case ZipCompression:
      {
        int
          code;

        zip_info.zalloc=AcquireZIPMemory;
        zip_info.zfree=RelinquishZIPMemory;
        zip_info.opaque=(voidpf) image;
        code=inflateInit(&zip_info);
        if (code != Z_OK)
          status=MagickFalse;
        break;
      }
#endif
      case RLECompression:
      {
        pixel.opacity=(Quantum) TransparentOpacity;
        index=(IndexPacket) 0;
        break;
      }
      default:
        break;
    }
    pixels=GetQuantumPixels(quantum_info);
    index=(IndexPacket) 0;
    length=0;
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      register IndexPacket
        *magick_restrict indexes;

      register ssize_t
        x;

      register PixelPacket
        *magick_restrict q;

      if (status == MagickFalse)
        break;
      q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
      if (q == (PixelPacket *) NULL)
        break;
      indexes=GetAuthenticIndexQueue(image);
      switch (image->compression)
      {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
        case BZipCompression:
        {
          bzip_info.next_out=(char *) pixels;
          bzip_info.avail_out=(unsigned int) (packet_size*image->columns);
          do
          {
            int
              code;

            if (bzip_info.avail_in == 0)
              {
                bzip_info.next_in=(char *) compress_pixels;
                length=(size_t) BZipMaxExtent(packet_size*image->columns);
                if (version != 0.0)
                  length=(size_t) ReadBlobMSBLong(image);
                if (length < compress_extent)
                  bzip_info.avail_in=(unsigned int) ReadBlob(image,length,
                    (unsigned char *) bzip_info.next_in);
                if ((length > compress_extent) ||
                    ((size_t) bzip_info.avail_in != length))
                  {
                    (void) BZ2_bzDecompressEnd(&bzip_info);
                    ThrowMIFFException(CorruptImageError,
                      "UnableToReadImageData");
                  }
              }
            code=BZ2_bzDecompress(&bzip_info);
            if ((code != BZ_OK) && (code != BZ_STREAM_END))
              {
                status=MagickFalse;
                break;
              }
            if (code == BZ_STREAM_END)
              break;
          } while (bzip_info.avail_out != 0);
          (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          break;
        }
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
        case LZMACompression:
        {
          lzma_info.next_out=pixels;
          lzma_info.avail_out=packet_size*image->columns;
          do
          {
            int
              code;

            if (lzma_info.avail_in == 0)
              {
                lzma_info.next_in=compress_pixels;
                length=(size_t) ReadBlobMSBLong(image);
                if (length <= compress_extent)
                  lzma_info.avail_in=(unsigned int) ReadBlob(image,length,
                    (unsigned char *) lzma_info.next_in);
                if ((length > compress_extent) ||
                    (lzma_info.avail_in != length))
                  {
                    lzma_end(&lzma_info);
                    ThrowMIFFException(CorruptImageError,
                      "UnableToReadImageData");
                  }
              }
            code=lzma_code(&lzma_info,LZMA_RUN);
            if ((code != LZMA_OK) && (code != LZMA_STREAM_END))
              {
                status=MagickFalse;
                break;
              }
            if (code == LZMA_STREAM_END)
              break;
          } while (lzma_info.avail_out != 0);
          (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          break;
        }
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
        case LZWCompression:
        case ZipCompression:
        {
          zip_info.next_out=pixels;
          zip_info.avail_out=(uInt) (packet_size*image->columns);
          do
          {
            int
              code;

            if (zip_info.avail_in == 0)
              {
                zip_info.next_in=compress_pixels;
                length=(size_t) ZipMaxExtent(packet_size*image->columns);
                if (version != 0.0)
                  length=(size_t) ReadBlobMSBLong(image);
                if (length <= compress_extent)
                  zip_info.avail_in=(unsigned int) ReadBlob(image,length,
                    zip_info.next_in);
                if ((length > compress_extent) ||
                    ((size_t) zip_info.avail_in != length))
                  {
                    (void) inflateEnd(&zip_info);
                    ThrowMIFFException(CorruptImageError,
                      "UnableToReadImageData");
                  }
              }
            code=inflate(&zip_info,Z_SYNC_FLUSH);
            if ((code != Z_OK) && (code != Z_STREAM_END))
              {
                status=MagickFalse;
                break;
              }
            if (code == Z_STREAM_END)
              break;
          } while (zip_info.avail_out != 0);
          (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          break;
        }
#endif
        case RLECompression:
        {
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (length == 0)
              {
                count=ReadBlob(image,packet_size,pixels);
                if (count != packet_size)
                  ThrowMIFFException(CorruptImageError,"UnableToReadImageData");
                PushRunlengthPacket(image,pixels,&length,&pixel,&index);
              }
            length--;
            if ((image->storage_class == PseudoClass) ||
                (image->colorspace == CMYKColorspace))
              SetPixelIndex(indexes+x,index);
            SetPixelRed(q,pixel.red);
            SetPixelGreen(q,pixel.green);
            SetPixelBlue(q,pixel.blue);
            SetPixelOpacity(q,pixel.opacity);
            q++;
          }
          break;
        }
        default:
        {
          count=ReadBlob(image,packet_size*image->columns,pixels);
          if (count != (packet_size*image->columns))
            ThrowMIFFException(CorruptImageError,"UnableToReadImageData");
          (void) ImportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          break;
        }
      }
      if (SyncAuthenticPixels(image,exception) == MagickFalse)
        break;
    }
    SetQuantumImageType(image,quantum_type);
    switch (image->compression)
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      case BZipCompression:
      {
        int
          code;

        if (version == 0.0)
          {
            MagickOffsetType
              offset;

            offset=SeekBlob(image,-((MagickOffsetType) bzip_info.avail_in),
              SEEK_CUR);
            if (offset < 0)
              {
                (void) BZ2_bzDecompressEnd(&bzip_info);
                ThrowMIFFException(CorruptImageError,"ImproperImageHeader");
              }
          }
        code=BZ2_bzDecompressEnd(&bzip_info);
        if (code != BZ_OK)
          status=MagickFalse;
        break;
      }
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
      case LZMACompression:
      {
        int
          code;

        code=lzma_code(&lzma_info,LZMA_FINISH);
        if ((code != LZMA_STREAM_END) && (code != LZMA_OK))
          status=MagickFalse;
        lzma_end(&lzma_info);
        break;
      }
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      case LZWCompression:
      case ZipCompression:
      {
        int
          code;

        if (version == 0.0)
          {
            MagickOffsetType
              offset;

            offset=SeekBlob(image,-((MagickOffsetType) zip_info.avail_in),
              SEEK_CUR);
            if (offset < 0)
              {
                (void) inflateEnd(&zip_info);
                ThrowMIFFException(CorruptImageError,"ImproperImageHeader");
              }
          }
        code=inflateEnd(&zip_info);
        if (code != Z_OK)
          status=MagickFalse;
        break;
      }
#endif
      default:
        break;
    }
    quantum_info=DestroyQuantumInfo(quantum_info);
    compress_pixels=(unsigned char *) RelinquishMagickMemory(compress_pixels);
    if (((y != (ssize_t) image->rows)) || (status == MagickFalse))
      {
        image=DestroyImageList(image);
        return((Image *) NULL);
      }
    if (EOFBlob(image) != MagickFalse)
      {
        ThrowFileException(exception,CorruptImageError,"UnexpectedEndOfFile",
          image->filename);
        break;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    do
    {
      c=ReadBlobByte(image);
    } while ((isgraph(c) == MagickFalse) && (c != EOF));
    if (c != EOF)
      {
        /*
          Allocate next image structure.
        */
        AcquireNextImage(image_info,image);
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
  } while (c != EOF);
  (void) CloseBlob(image);
  if (status == MagickFalse)
    return(DestroyImageList(image));
  return(GetFirstImageInList(image));
}