static Image *ReadTIFFImage(const ImageInfo *image_info,
  ExceptionInfo *exception)
{
#define MaxPixelChannels  32
#define ThrowTIFFException(severity,message) \
{ \
  if (pixel_info != (MemoryInfo *) NULL) \
    pixel_info=RelinquishVirtualMemory(pixel_info); \
  if (quantum_info != (QuantumInfo *) NULL) \
    quantum_info=DestroyQuantumInfo(quantum_info); \
  TIFFClose(tiff); \
  ThrowReaderException(severity,message); \
}

  const char
    *option;

  float
    *chromaticity,
    x_position,
    y_position,
    x_resolution,
    y_resolution;

  Image
    *image;

  int
    tiff_status;

  MagickBooleanType
    more_frames;

  MagickStatusType
    status;

  MemoryInfo
    *pixel_info = (MemoryInfo *) NULL;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  ssize_t
    i;

  size_t
    number_pixels;

  ssize_t
    y;

  TIFF
    *tiff;

  TIFFMethodType
    method;

  uint16
    compress_tag,
    bits_per_sample,
    endian,
    extra_samples,
    interlace,
    max_sample_value,
    min_sample_value,
    orientation,
    pages,
    photometric,
    *sample_info,
    sample_format,
    samples_per_pixel,
    units,
    value;

  uint32
    height,
    rows_per_strip,
    width;

  unsigned char
    *pixels;

  void
    *sans[4] = { NULL, NULL, NULL, NULL };

  /*
    Open image.
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
  (void) SetMagickThreadValue(tiff_exception,exception);
  tiff=TIFFClientOpen(image->filename,"rb",(thandle_t) image,TIFFReadBlob,
    TIFFWriteBlob,TIFFSeekBlob,TIFFCloseBlob,TIFFGetBlobSize,TIFFMapBlob,
    TIFFUnmapBlob);
  if (tiff == (TIFF *) NULL)
    {
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if (exception->severity > ErrorException)
    {
      TIFFClose(tiff);
      image=DestroyImageList(image);
      return((Image *) NULL);
    }
  if (image_info->number_scenes != 0)
    {
      /*
        Generate blank images for subimage specification (e.g. image.tif[4].
        We need to check the number of directores because it is possible that
        the subimage(s) are stored in the photoshop profile.
      */
      if (image_info->scene < (size_t)TIFFNumberOfDirectories(tiff))
        {
          for (i=0; i < (ssize_t) image_info->scene; i++)
          {
            status=TIFFReadDirectory(tiff) != 0 ? MagickTrue : MagickFalse;
            if (status == MagickFalse)
              {
                TIFFClose(tiff);
                image=DestroyImageList(image);
                return((Image *) NULL);
              }
            AcquireNextImage(image_info,image);
            if (GetNextImageInList(image) == (Image *) NULL)
              {
                TIFFClose(tiff);
                image=DestroyImageList(image);
                return((Image *) NULL);
              }
            image=SyncNextImageInList(image);
          }
        }
    }
  more_frames=MagickTrue;
  do
  {
    /* TIFFPrintDirectory(tiff,stdout,MagickFalse); */
    photometric=PHOTOMETRIC_RGB;
    if ((TIFFGetField(tiff,TIFFTAG_IMAGEWIDTH,&width) != 1) ||
        (TIFFGetField(tiff,TIFFTAG_IMAGELENGTH,&height) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_PHOTOMETRIC,&photometric,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_COMPRESSION,&compress_tag,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_FILLORDER,&endian,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_PLANARCONFIG,&interlace,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLESPERPIXEL,&samples_per_pixel,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_BITSPERSAMPLE,&bits_per_sample,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_SAMPLEFORMAT,&sample_format,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_MINSAMPLEVALUE,&min_sample_value,sans) != 1) ||
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_MAXSAMPLEVALUE,&max_sample_value,sans) != 1))
      {
        TIFFClose(tiff);
        ThrowReaderException(CorruptImageError,"ImproperImageHeader");
      }
    if (((sample_format != SAMPLEFORMAT_IEEEFP) || (bits_per_sample != 64)) &&
        ((bits_per_sample <= 0) || (bits_per_sample > 32)))
      {
        TIFFClose(tiff);
        ThrowReaderException(CorruptImageError,"UnsupportedBitsPerPixel");
      }
    if (samples_per_pixel > MaxPixelChannels)
      {
        TIFFClose(tiff);
        ThrowReaderException(CorruptImageError,"MaximumChannelsExceeded");
      }
    if (sample_format == SAMPLEFORMAT_IEEEFP)
      (void) SetImageProperty(image,"quantum:format","floating-point");
    switch (photometric)
    {
      case PHOTOMETRIC_MINISBLACK:
      {
        (void) SetImageProperty(image,"tiff:photometric","min-is-black");
        break;
      }
      case PHOTOMETRIC_MINISWHITE:
      {
        (void) SetImageProperty(image,"tiff:photometric","min-is-white");
        break;
      }
      case PHOTOMETRIC_PALETTE:
      {
        (void) SetImageProperty(image,"tiff:photometric","palette");
        break;
      }
      case PHOTOMETRIC_RGB:
      {
        (void) SetImageProperty(image,"tiff:photometric","RGB");
        break;
      }
      case PHOTOMETRIC_CIELAB:
      {
        (void) SetImageProperty(image,"tiff:photometric","CIELAB");
        break;
      }
      case PHOTOMETRIC_LOGL:
      {
        (void) SetImageProperty(image,"tiff:photometric","CIE Log2(L)");
        break;
      }
      case PHOTOMETRIC_LOGLUV:
      {
        (void) SetImageProperty(image,"tiff:photometric","LOGLUV");
        break;
      }
#if defined(PHOTOMETRIC_MASK)
      case PHOTOMETRIC_MASK:
      {
        (void) SetImageProperty(image,"tiff:photometric","MASK");
        break;
      }
#endif
      case PHOTOMETRIC_SEPARATED:
      {
        (void) SetImageProperty(image,"tiff:photometric","separated");
        break;
      }
      case PHOTOMETRIC_YCBCR:
      {
        (void) SetImageProperty(image,"tiff:photometric","YCBCR");
        break;
      }
      default:
      {
        (void) SetImageProperty(image,"tiff:photometric","unknown");
        break;
      }
    }
    if (image->debug != MagickFalse)
      {
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Geometry: %ux%u",
          (unsigned int) width,(unsigned int) height);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Interlace: %u",
          interlace);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Bits per sample: %u",bits_per_sample);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Min sample value: %u",min_sample_value);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),
          "Max sample value: %u",max_sample_value);
        (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Photometric "
          "interpretation: %s",GetImageProperty(image,"tiff:photometric"));
      }
    image->columns=(size_t) width;
    image->rows=(size_t) height;
    image->depth=(size_t) bits_per_sample;
    if (image->debug != MagickFalse)
      (void) LogMagickEvent(CoderEvent,GetMagickModule(),"Image depth: %.20g",
        (double) image->depth);
    image->endian=MSBEndian;
    if (endian == FILLORDER_LSB2MSB)
      image->endian=LSBEndian;
#if defined(MAGICKCORE_HAVE_TIFFISBIGENDIAN)
    if (TIFFIsBigEndian(tiff) == 0)
      {
        (void) SetImageProperty(image,"tiff:endian","lsb");
        image->endian=LSBEndian;
      }
    else
      {
        (void) SetImageProperty(image,"tiff:endian","msb");
        image->endian=MSBEndian;
      }
#endif
    if ((photometric == PHOTOMETRIC_MINISBLACK) ||
        (photometric == PHOTOMETRIC_MINISWHITE))
      image->colorspace=GRAYColorspace;
    if (photometric == PHOTOMETRIC_SEPARATED)
      image->colorspace=CMYKColorspace;
    if (photometric == PHOTOMETRIC_CIELAB)
      image->colorspace=LabColorspace;
    if ((photometric == PHOTOMETRIC_YCBCR) && (compress_tag != COMPRESSION_JPEG))
      image->colorspace=YCbCrColorspace;
    status=TIFFGetProfiles(tiff,image);
    if (status == MagickFalse)
      {
        TIFFClose(tiff);
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    status=TIFFGetProperties(tiff,image);
    if (status == MagickFalse)
      {
        TIFFClose(tiff);
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    option=GetImageOption(image_info,"tiff:exif-properties");
    if ((option == (const char *) NULL) ||
        (IsMagickTrue(option) != MagickFalse))
      (void) TIFFGetEXIFProperties(tiff,image);
    option=GetImageOption(image_info,"tiff:gps-properties");
    if ((option == (const char *) NULL) ||
        (IsMagickTrue(option) != MagickFalse))
      (void) TIFFGetGPSProperties(tiff,image);
    if ((TIFFGetFieldDefaulted(tiff,TIFFTAG_XRESOLUTION,&x_resolution,sans) == 1) &&
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_YRESOLUTION,&y_resolution,sans) == 1))
      {
        image->x_resolution=x_resolution;
        image->y_resolution=y_resolution;
      }
    if (TIFFGetFieldDefaulted(tiff,TIFFTAG_RESOLUTIONUNIT,&units,sans,sans) == 1)
      {
        if (units == RESUNIT_INCH)
          image->units=PixelsPerInchResolution;
        if (units == RESUNIT_CENTIMETER)
          image->units=PixelsPerCentimeterResolution;
      }
    if ((TIFFGetFieldDefaulted(tiff,TIFFTAG_XPOSITION,&x_position,sans) == 1) &&
        (TIFFGetFieldDefaulted(tiff,TIFFTAG_YPOSITION,&y_position,sans) == 1))
      {
        image->page.x=CastDoubleToLong(ceil(x_position*
          image->x_resolution-0.5));
        image->page.y=CastDoubleToLong(ceil(y_position*
          image->y_resolution-0.5));
      }
    if (TIFFGetFieldDefaulted(tiff,TIFFTAG_ORIENTATION,&orientation,sans) == 1)
      image->orientation=(OrientationType) orientation;
    if (TIFFGetField(tiff,TIFFTAG_WHITEPOINT,&chromaticity) == 1)
      {
        if ((chromaticity != (float *) NULL) && (*chromaticity != 0.0))
          {
            image->chromaticity.white_point.x=chromaticity[0];
            image->chromaticity.white_point.y=chromaticity[1];
          }
      }
    if (TIFFGetField(tiff,TIFFTAG_PRIMARYCHROMATICITIES,&chromaticity) == 1)
      {
        if ((chromaticity != (float *) NULL) && (*chromaticity != 0.0))
          {
            image->chromaticity.red_primary.x=chromaticity[0];
            image->chromaticity.red_primary.y=chromaticity[1];
            image->chromaticity.green_primary.x=chromaticity[2];
            image->chromaticity.green_primary.y=chromaticity[3];
            image->chromaticity.blue_primary.x=chromaticity[4];
            image->chromaticity.blue_primary.y=chromaticity[5];
          }
      }
#if defined(MAGICKCORE_HAVE_TIFFISCODECCONFIGURED) || (TIFFLIB_VERSION > 20040919)
    if ((compress_tag != COMPRESSION_NONE) &&
        (TIFFIsCODECConfigured(compress_tag) == 0))
      {
        TIFFClose(tiff);
        ThrowReaderException(CoderError,"CompressNotSupported");
      }
#endif
    switch (compress_tag)
    {
      case COMPRESSION_NONE: image->compression=NoCompression; break;
      case COMPRESSION_CCITTFAX3: image->compression=FaxCompression; break;
      case COMPRESSION_CCITTFAX4: image->compression=Group4Compression; break;
      case COMPRESSION_JPEG:
      {
         image->compression=JPEGCompression;
#if defined(JPEG_SUPPORT)
         {
           char
             sampling_factor[MaxTextExtent];

           int
             tiff_status;

           uint16
             horizontal,
             vertical;

           tiff_status=TIFFGetField(tiff,TIFFTAG_YCBCRSUBSAMPLING,&horizontal,
             &vertical);
           if (tiff_status == 1)
             {
               (void) FormatLocaleString(sampling_factor,MaxTextExtent,"%dx%d",
                 horizontal,vertical);
               (void) SetImageProperty(image,"jpeg:sampling-factor",
                 sampling_factor);
               (void) LogMagickEvent(CoderEvent,GetMagickModule(),
                 "Sampling Factors: %s",sampling_factor);
             }
         }
#endif
        break;
      }
      case COMPRESSION_OJPEG: image->compression=JPEGCompression; break;
#if defined(COMPRESSION_LZMA)
      case COMPRESSION_LZMA: image->compression=LZMACompression; break;
#endif
      case COMPRESSION_LZW: image->compression=LZWCompression; break;
      case COMPRESSION_DEFLATE: image->compression=ZipCompression; break;
      case COMPRESSION_ADOBE_DEFLATE: image->compression=ZipCompression; break;
#if defined(COMPRESSION_WEBP)
      case COMPRESSION_WEBP: image->compression=WebPCompression; break;
#endif
#if defined(COMPRESSION_ZSTD)
      case COMPRESSION_ZSTD: image->compression=ZstdCompression; break;
#endif
      default: image->compression=RLECompression; break;
    }
    quantum_info=(QuantumInfo *) NULL;
    if ((photometric == PHOTOMETRIC_PALETTE) &&
        (pow(2.0,1.0*bits_per_sample) <= MaxColormapSize))
      {
        size_t
          colors;

        colors=(size_t) GetQuantumRange(bits_per_sample)+1;
        if (AcquireImageColormap(image,colors) == MagickFalse)
          {
            TIFFClose(tiff);
            ThrowReaderException(ResourceLimitError,"MemoryAllocationFailed");
          }
      }
    if (TIFFGetFieldDefaulted(tiff,TIFFTAG_PAGENUMBER,&value,&pages,sans) == 1)
      image->scene=value;
    if (image->storage_class == PseudoClass)
      {
        int
          tiff_status;

        size_t
          range;

        uint16
          *blue_colormap,
          *green_colormap,
          *red_colormap;

        /*
          Initialize colormap.
        */
        tiff_status=TIFFGetField(tiff,TIFFTAG_COLORMAP,&red_colormap,
          &green_colormap,&blue_colormap);
        if (tiff_status == 1)
          {
            if ((red_colormap != (uint16 *) NULL) &&
                (green_colormap != (uint16 *) NULL) &&
                (blue_colormap != (uint16 *) NULL))
              {
                range=255;  /* might be old style 8-bit colormap */
                for (i=0; i < (ssize_t) image->colors; i++)
                  if ((red_colormap[i] >= 256) || (green_colormap[i] >= 256) ||
                      (blue_colormap[i] >= 256))
                    {
                      range=65535;
                      break;
                    }
                for (i=0; i < (ssize_t) image->colors; i++)
                {
                  image->colormap[i].red=ClampToQuantum(((double)
                    QuantumRange*red_colormap[i])/range);
                  image->colormap[i].green=ClampToQuantum(((double)
                    QuantumRange*green_colormap[i])/range);
                  image->colormap[i].blue=ClampToQuantum(((double)
                    QuantumRange*blue_colormap[i])/range);
                }
              }
          }
      }
    if (image_info->ping != MagickFalse)
      {
        if (image_info->number_scenes != 0)
          if (image->scene >= (image_info->scene+image_info->number_scenes-1))
            break;
        goto next_tiff_frame;
      }
    status=SetImageExtent(image,image->columns,image->rows);
    if (status == MagickFalse)
      {
        TIFFClose(tiff);
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    status=SetImageColorspace(image,image->colorspace);
    status&=ResetImagePixels(image,exception);
    if (status == MagickFalse)
      {
        TIFFClose(tiff);
        InheritException(exception,&image->exception);
        return(DestroyImageList(image));
      }
    /*
      Allocate memory for the image and pixel buffer.
    */
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
    if (sample_format == SAMPLEFORMAT_UINT)
      status=SetQuantumFormat(image,quantum_info,UnsignedQuantumFormat);
    if (sample_format == SAMPLEFORMAT_INT)
      status=SetQuantumFormat(image,quantum_info,SignedQuantumFormat);
    if (sample_format == SAMPLEFORMAT_IEEEFP)
      status=SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
    if (status == MagickFalse)
      ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
    status=MagickTrue;
    switch (photometric)
    {
      case PHOTOMETRIC_MINISBLACK:
      {
        quantum_info->min_is_white=MagickFalse;
        break;
      }
      case PHOTOMETRIC_MINISWHITE:
      {
        quantum_info->min_is_white=MagickTrue;
        break;
      }
      default:
        break;
    }
    extra_samples=0;
    tiff_status=TIFFGetFieldDefaulted(tiff,TIFFTAG_EXTRASAMPLES,&extra_samples,
      &sample_info,sans);
    if (tiff_status == 1)
      {
        (void) SetImageProperty(image,"tiff:alpha","unspecified");
        if (extra_samples == 0)
          {
            if ((samples_per_pixel == 4) && (photometric == PHOTOMETRIC_RGB))
              image->matte=MagickTrue;
          }
        else
          for (i=0; i < extra_samples; i++)
          {
            image->matte=MagickTrue;
            if (sample_info[i] == EXTRASAMPLE_ASSOCALPHA)
              {
                SetQuantumAlphaType(quantum_info,AssociatedQuantumAlpha);
                (void) SetImageProperty(image,"tiff:alpha","associated");
              }
            else
              if (sample_info[i] == EXTRASAMPLE_UNASSALPHA)
                {
                  SetQuantumAlphaType(quantum_info,DisassociatedQuantumAlpha);
                  (void) SetImageProperty(image,"tiff:alpha","unassociated");
                }
          }
      }
    if (image->matte != MagickFalse)
      (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
    method=ReadGenericMethod;
    rows_per_strip=(uint32) image->rows;
    if (TIFFGetField(tiff,TIFFTAG_ROWSPERSTRIP,&rows_per_strip) == 1)
      {
        char
          value[MaxTextExtent];

        (void) FormatLocaleString(value,MaxTextExtent,"%u",(unsigned int)
          rows_per_strip);
        (void) SetImageProperty(image,"tiff:rows-per-strip",value);
        method=ReadStripMethod;
        if (rows_per_strip > (uint32) image->rows)
          rows_per_strip=(uint32) image->rows;
      }
    if (TIFFIsTiled(tiff) != MagickFalse)
      {
        uint32
          columns,
          rows;

        if ((TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&columns) != 1) ||
            (TIFFGetField(tiff,TIFFTAG_TILELENGTH,&rows) != 1))
          ThrowTIFFException(CoderError,"ImageIsNotTiled");
        if ((AcquireMagickResource(WidthResource,columns) == MagickFalse) ||
            (AcquireMagickResource(HeightResource,rows) == MagickFalse))
          ThrowTIFFException(ImageError,"WidthOrHeightExceedsLimit");
        method=ReadTileMethod;
      }
    if ((photometric == PHOTOMETRIC_LOGLUV) ||
        (compress_tag == COMPRESSION_CCITTFAX3))
      method=ReadGenericMethod;
    if (image->compression == JPEGCompression)
      method=GetJPEGMethod(image,tiff,photometric,bits_per_sample,
        samples_per_pixel);
    quantum_info->endian=LSBEndian;
    if (TIFFScanlineSize(tiff) <= 0)
      ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
    if (((MagickSizeType) TIFFScanlineSize(tiff)) > (2.53*GetBlobSize(image)))
      ThrowTIFFException(CorruptImageError,"InsufficientImageDataInFile");
    number_pixels=MagickMax(TIFFScanlineSize(tiff),MagickMax((ssize_t)
      image->columns*samples_per_pixel*pow(2.0,ceil(log(bits_per_sample)/
      log(2.0))),image->columns*rows_per_strip));
    pixel_info=AcquireVirtualMemory(number_pixels,sizeof(uint32));
    if (pixel_info == (MemoryInfo *) NULL)
      ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
    pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
    (void) ResetMagickMemory(pixels,0,number_pixels*sizeof(uint32));
    quantum_type=GrayQuantum;
    if (image->storage_class == PseudoClass)
      quantum_type=IndexQuantum;
    if (interlace != PLANARCONFIG_SEPARATE)
      {
        size_t
          pad;

        pad=(size_t) MagickMax((ssize_t) samples_per_pixel-1,0);
        if (image->matte != MagickFalse)
          {
            if (image->storage_class == PseudoClass)
              quantum_type=IndexAlphaQuantum;
            else
              quantum_type=samples_per_pixel == 1 ? AlphaQuantum :
                GrayAlphaQuantum;
          }
        if ((samples_per_pixel > 2) && (interlace != PLANARCONFIG_SEPARATE))
          {
            quantum_type=RGBQuantum;
            pad=(size_t) MagickMax((size_t) samples_per_pixel-3,0);
            if (image->matte != MagickFalse)
              {
                quantum_type=RGBAQuantum;
                pad=(size_t) MagickMax((size_t) samples_per_pixel-4,0);
              }
            if (image->colorspace == CMYKColorspace)
              {
                quantum_type=CMYKQuantum;
                pad=(size_t) MagickMax((size_t) samples_per_pixel-4,0);
                if (image->matte != MagickFalse)
                  {
                    quantum_type=CMYKAQuantum;
                    pad=(size_t) MagickMax((size_t) samples_per_pixel-5,0);
                  }
              }
            status=SetQuantumPad(image,quantum_info,pad*((bits_per_sample+7) >>
              3));
            if (status == MagickFalse)
              ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
          }
      }
    switch (method)
    {
      case ReadYCCKMethod:
      {
        /*
          Convert YCC TIFF image.
        */
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          int
            status;

          IndexPacket
            *indexes;

          PixelPacket
            *magick_restrict q;

          ssize_t
            x;

          unsigned char
            *p;

          status=TIFFReadPixels(tiff,0,y,(char *) pixels);
          if (status == -1)
            break;
          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          indexes=GetAuthenticIndexQueue(image);
          p=pixels;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelCyan(q,ScaleCharToQuantum(ClampYCC((double) *p+
              (1.402*(double) *(p+2))-179.456)));
            SetPixelMagenta(q,ScaleCharToQuantum(ClampYCC((double) *p-
              (0.34414*(double) *(p+1))-(0.71414*(double ) *(p+2))+
              135.45984)));
            SetPixelYellow(q,ScaleCharToQuantum(ClampYCC((double) *p+
              (1.772*(double) *(p+1))-226.816)));
            SetPixelBlack(indexes+x,ScaleCharToQuantum((unsigned char)*(p+3)));
            q++;
            p+=4;
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
        }
        break;
      }
      case ReadStripMethod:
      {
        unsigned char
          *p;

        size_t
          extent;

        ssize_t
          stride,
          strip_id;

        tsize_t
          strip_size;

        unsigned char
          *strip_pixels;

        /*
          Convert stripped TIFF image.
        */
        extent=2*TIFFStripSize(tiff);
#if defined(TIFF_VERSION_BIG)
        extent+=image->columns*sizeof(uint64);
#else
        extent+=image->columns*sizeof(uint32);
#endif
        strip_pixels=(unsigned char *) AcquireQuantumMemory(extent,
          sizeof(*strip_pixels));
        if (strip_pixels == (unsigned char *) NULL)
          ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
        (void) memset(strip_pixels,0,extent*sizeof(*strip_pixels));
        stride=TIFFVStripSize(tiff,1);
        strip_id=0;
        p=strip_pixels;
        for (i=0; i < (ssize_t) samples_per_pixel; i++)
        {
          size_t
            rows_remaining;

          switch (i)
          {
            case 0: break;
            case 1: quantum_type=GreenQuantum; break;
            case 2: quantum_type=BlueQuantum; break;
            case 3:
            {
              quantum_type=AlphaQuantum;
              if (image->colorspace == CMYKColorspace)
                quantum_type=BlackQuantum;
              break;
            }
            case 4: quantum_type=AlphaQuantum; break;
            default: break;
          }
          rows_remaining=0;
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            PixelPacket
              *magick_restrict q;

            q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
            if (q == (PixelPacket *) NULL)
              break;
            if (rows_remaining == 0)
              {
                strip_size=TIFFReadEncodedStrip(tiff,strip_id,strip_pixels,
                  TIFFStripSize(tiff));
                if (strip_size == -1)
                  break;
                rows_remaining=rows_per_strip;
                if ((y+rows_per_strip) > (ssize_t) image->rows)
                  rows_remaining=(rows_per_strip-(y+rows_per_strip-
                    image->rows));
                p=strip_pixels;
                strip_id++;
              }
            (void) ImportQuantumPixels(image,(CacheView *) NULL,
              quantum_info,quantum_type,p,exception);
            p+=stride;
            rows_remaining--;
            if (SyncAuthenticPixels(image,exception) == MagickFalse)
              break;
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) y,
                  image->rows);
                if (status == MagickFalse)
                  break;
              }
          }
         if ((samples_per_pixel > 1) && (interlace != PLANARCONFIG_SEPARATE))
            break;
        }
        strip_pixels=(unsigned char *) RelinquishMagickMemory(strip_pixels);
        break;
      }
      case ReadTileMethod:
      {
        unsigned char
          *p;

        size_t
          extent;

        uint32
          columns,
          rows;

        unsigned char
          *tile_pixels;

        /*
          Convert tiled TIFF image.
        */
        if ((TIFFGetField(tiff,TIFFTAG_TILEWIDTH,&columns) != 1) ||
            (TIFFGetField(tiff,TIFFTAG_TILELENGTH,&rows) != 1))
          ThrowTIFFException(CoderError,"ImageIsNotTiled");
        number_pixels=(MagickSizeType) columns*rows;
        if (HeapOverflowSanityCheck(rows,sizeof(*tile_pixels)) != MagickFalse)
          ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
        extent=TIFFTileSize(tiff);
#if defined(TIFF_VERSION_BIG)
        extent+=columns*sizeof(uint64);
#else
        extent+=columns*sizeof(uint32);
#endif
        tile_pixels=(unsigned char *) AcquireQuantumMemory(extent,
          sizeof(*tile_pixels));
        if (tile_pixels == (unsigned char *) NULL)
          ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
        (void) memset(tile_pixels,0,extent*sizeof(*tile_pixels));
        for (i=0; i < (ssize_t) samples_per_pixel; i++)
        {
          switch (i)
          {
            case 0: break;
            case 1: quantum_type=GreenQuantum; break;
            case 2: quantum_type=BlueQuantum; break;
            case 3:
            {
              quantum_type=AlphaQuantum;
              if (image->colorspace == CMYKColorspace)
                quantum_type=BlackQuantum;
              break;
            }
            case 4: quantum_type=AlphaQuantum; break;
            default: break;
          }
          for (y=0; y < (ssize_t) image->rows; y+=rows)
          {
            ssize_t
              x;

            size_t
              rows_remaining;

            rows_remaining=image->rows-y;
            if ((ssize_t) (y+rows) < (ssize_t) image->rows)
              rows_remaining=rows;
            for (x=0; x < (ssize_t) image->columns; x+=columns)
            {
              size_t
                columns_remaining,
                row;

              columns_remaining=image->columns-x;
              if ((ssize_t) (x+columns) < (ssize_t) image->columns)
                columns_remaining=columns;
              if (TIFFReadTile(tiff,tile_pixels,(uint32) x,(uint32) y,0,i) == 0)
                break;
              p=tile_pixels;
              for (row=0; row < rows_remaining; row++)
              {
                PixelPacket
                  *magick_restrict q;

                q=GetAuthenticPixels(image,x,y+row,columns_remaining,1,
                  exception);
                if (q == (PixelPacket *) NULL)
                  break;
                (void) ImportQuantumPixels(image,(CacheView *) NULL,
                  quantum_info,quantum_type,p,exception);
                p+=TIFFTileRowSize(tiff);
                if (SyncAuthenticPixels(image,exception) == MagickFalse)
                  break;
              }
            }
          }
          if ((samples_per_pixel > 1) && (interlace != PLANARCONFIG_SEPARATE))
            break;
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,LoadImageTag,(MagickOffsetType) i,
                samples_per_pixel);
              if (status == MagickFalse)
                break;
            }
        }
        tile_pixels=(unsigned char *) RelinquishMagickMemory(tile_pixels);
        break;
      }
      case ReadGenericMethod:
      default:
      {
        MemoryInfo
          *generic_info = (MemoryInfo *) NULL;

        uint32
          *p;

        uint32
          *pixels;

        /*
          Convert generic TIFF image.
        */
        if (HeapOverflowSanityCheck(image->rows,sizeof(*pixels)) != MagickFalse)
          ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
        number_pixels=(MagickSizeType) image->columns*image->rows;
#if defined(TIFF_VERSION_BIG)
        number_pixels+=image->columns*sizeof(uint64);
#else
        number_pixels+=image->columns*sizeof(uint32);
#endif
        generic_info=AcquireVirtualMemory(number_pixels,sizeof(*pixels));
        if (generic_info == (MemoryInfo *) NULL)
          ThrowTIFFException(ResourceLimitError,"MemoryAllocationFailed");
        pixels=(uint32 *) GetVirtualMemoryBlob(generic_info);
        (void) TIFFReadRGBAImage(tiff,(uint32) image->columns,(uint32)
          image->rows,(uint32 *) pixels,0);
        p=pixels+(image->columns*image->rows)-1;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          ssize_t
            x;

          PixelPacket
            *magick_restrict q;

          q=QueueAuthenticPixels(image,0,y,image->columns,1,exception);
          if (q == (PixelPacket *) NULL)
            break;
          q+=image->columns-1;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            SetPixelRed(q,ScaleCharToQuantum((unsigned char) TIFFGetR(*p)));
            SetPixelGreen(q,ScaleCharToQuantum((unsigned char) TIFFGetG(*p)));
            SetPixelBlue(q,ScaleCharToQuantum((unsigned char) TIFFGetB(*p)));
            if (image->matte == MagickFalse)
              SetPixelOpacity(q,OpaqueOpacity);
            else
              SetPixelAlpha(q,ScaleCharToQuantum((unsigned char) TIFFGetA(*p)));
            p--;
            q--;
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
        }
        generic_info=RelinquishVirtualMemory(generic_info);
        break;
      }
    }
    pixel_info=RelinquishVirtualMemory(pixel_info);
    SetQuantumImageType(image,quantum_type);
  next_tiff_frame:
    if (quantum_info != (QuantumInfo *) NULL)
      quantum_info=DestroyQuantumInfo(quantum_info);
    if (photometric == PHOTOMETRIC_CIELAB)
      DecodeLabImage(image,exception);
    if ((photometric == PHOTOMETRIC_LOGL) ||
        (photometric == PHOTOMETRIC_MINISBLACK) ||
        (photometric == PHOTOMETRIC_MINISWHITE))
      {
        image->type=GrayscaleType;
        if (bits_per_sample == 1)
          image->type=BilevelType;
      }
    /*
      Proceed to next image.
    */
    if (image_info->number_scenes != 0)
      if (image->scene >= (image_info->scene+image_info->number_scenes-1))
        break;
    more_frames=TIFFReadDirectory(tiff) != 0 ? MagickTrue : MagickFalse;
    if (more_frames != MagickFalse)
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
        status=SetImageProgress(image,LoadImagesTag,image->scene-1,
          image->scene);
        if (status == MagickFalse)
          break;
      }
  } while ((status != MagickFalse) && (more_frames != MagickFalse));
  TIFFClose(tiff);
  if ((image_info->number_scenes != 0) &&
      (image_info->scene >= GetImageListLength(image)))
    status=MagickFalse;
  if (status == MagickFalse)
    return(DestroyImageList(image));
  TIFFReadPhotoshopLayers(image_info,image,exception);
  return(GetFirstImageInList(image));
}