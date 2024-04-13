MagickPrivate const Quantum *GetVirtualPixelsFromNexus(const Image *image,
  const VirtualPixelMethod virtual_pixel_method,const ssize_t x,const ssize_t y,
  const size_t columns,const size_t rows,NexusInfo *nexus_info,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info;

  MagickOffsetType
    offset;

  MagickSizeType
    length,
    number_pixels;

  NexusInfo
    **magick_restrict virtual_nexus;

  Quantum
    *magick_restrict pixels,
    virtual_pixel[MaxPixelChannels];

  RectangleInfo
    region;

  register const Quantum
    *magick_restrict p;

  register const void
    *magick_restrict r;

  register Quantum
    *magick_restrict q;

  register ssize_t
    i,
    u;

  register unsigned char
    *magick_restrict s;

  ssize_t
    v;

  void
    *magick_restrict virtual_metacontent;

  /*
    Acquire pixels.
  */
  assert(image != (const Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  assert(image->cache != (Cache) NULL);
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  if (cache_info->type == UndefinedCache)
    return((const Quantum *) NULL);
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  CopyOpenCLBuffer(cache_info);
#endif
  region.x=x;
  region.y=y;
  region.width=columns;
  region.height=rows;
  pixels=SetPixelCacheNexusPixels(cache_info,ReadMode,&region,nexus_info,
    exception);
  if (pixels == (Quantum *) NULL)
    return((const Quantum *) NULL);
  q=pixels;
  offset=(MagickOffsetType) nexus_info->region.y*cache_info->columns+
    nexus_info->region.x;
  length=(MagickSizeType) (nexus_info->region.height-1L)*cache_info->columns+
    nexus_info->region.width-1L;
  number_pixels=(MagickSizeType) cache_info->columns*cache_info->rows;
  if ((offset >= 0) && (((MagickSizeType) offset+length) < number_pixels))
    if ((x >= 0) && ((ssize_t) (x+columns) <= (ssize_t) cache_info->columns) &&
        (y >= 0) && ((ssize_t) (y+rows) <= (ssize_t) cache_info->rows))
      {
        MagickBooleanType
          status;

        /*
          Pixel request is inside cache extents.
        */
        if (nexus_info->authentic_pixel_cache != MagickFalse)
          return(q);
        status=ReadPixelCachePixels(cache_info,nexus_info,exception);
        if (status == MagickFalse)
          return((const Quantum *) NULL);
        if (cache_info->metacontent_extent != 0)
          {
            status=ReadPixelCacheMetacontent(cache_info,nexus_info,exception);
            if (status == MagickFalse)
              return((const Quantum *) NULL);
          }
        return(q);
      }
  /*
    Pixel request is outside cache extents.
  */
  s=(unsigned char *) nexus_info->metacontent;
  virtual_nexus=AcquirePixelCacheNexus(1);
  if (virtual_nexus == (NexusInfo **) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),CacheError,
        "UnableToGetCacheNexus","`%s'",image->filename);
      return((const Quantum *) NULL);
    }
  (void) ResetMagickMemory(virtual_pixel,0,cache_info->number_channels*
    sizeof(*virtual_pixel));
  virtual_metacontent=(void *) NULL;
  switch (virtual_pixel_method)
  {
    case BackgroundVirtualPixelMethod:
    case BlackVirtualPixelMethod:
    case GrayVirtualPixelMethod:
    case TransparentVirtualPixelMethod:
    case MaskVirtualPixelMethod:
    case WhiteVirtualPixelMethod:
    case EdgeVirtualPixelMethod:
    case CheckerTileVirtualPixelMethod:
    case HorizontalTileVirtualPixelMethod:
    case VerticalTileVirtualPixelMethod:
    {
      if (cache_info->metacontent_extent != 0)
        {
          /*
            Acquire a metacontent buffer.
          */
          virtual_metacontent=(void *) AcquireQuantumMemory(1,
            cache_info->metacontent_extent);
          if (virtual_metacontent == (void *) NULL)
            {
              virtual_nexus=DestroyPixelCacheNexus(virtual_nexus,1);
              (void) ThrowMagickException(exception,GetMagickModule(),
                CacheError,"UnableToGetCacheNexus","`%s'",image->filename);
              return((const Quantum *) NULL);
            }
          (void) ResetMagickMemory(virtual_metacontent,0,
            cache_info->metacontent_extent);
        }
      switch (virtual_pixel_method)
      {
        case BlackVirtualPixelMethod:
        {
          for (i=0; i < (ssize_t) cache_info->number_channels; i++)
            SetPixelChannel(image,(PixelChannel) i,(Quantum) 0,virtual_pixel);
          SetPixelAlpha(image,OpaqueAlpha,virtual_pixel);
          break;
        }
        case GrayVirtualPixelMethod:
        {
          for (i=0; i < (ssize_t) cache_info->number_channels; i++)
            SetPixelChannel(image,(PixelChannel) i,QuantumRange/2,
              virtual_pixel);
          SetPixelAlpha(image,OpaqueAlpha,virtual_pixel);
          break;
        }
        case TransparentVirtualPixelMethod:
        {
          for (i=0; i < (ssize_t) cache_info->number_channels; i++)
            SetPixelChannel(image,(PixelChannel) i,(Quantum) 0,virtual_pixel);
          SetPixelAlpha(image,TransparentAlpha,virtual_pixel);
          break;
        }
        case MaskVirtualPixelMethod:
        case WhiteVirtualPixelMethod:
        {
          for (i=0; i < (ssize_t) cache_info->number_channels; i++)
            SetPixelChannel(image,(PixelChannel) i,QuantumRange,virtual_pixel);
          SetPixelAlpha(image,OpaqueAlpha,virtual_pixel);
          break;
        }
        default:
        {
          SetPixelRed(image,ClampToQuantum(image->background_color.red),
            virtual_pixel);
          SetPixelGreen(image,ClampToQuantum(image->background_color.green),
            virtual_pixel);
          SetPixelBlue(image,ClampToQuantum(image->background_color.blue),
            virtual_pixel);
          SetPixelBlack(image,ClampToQuantum(image->background_color.black),
            virtual_pixel);
          SetPixelAlpha(image,ClampToQuantum(image->background_color.alpha),
            virtual_pixel);
          break;
        }
      }
      break;
    }
    default:
      break;
  }
  for (v=0; v < (ssize_t) rows; v++)
  {
    ssize_t
      y_offset;

    y_offset=y+v;
    if ((virtual_pixel_method == EdgeVirtualPixelMethod) ||
        (virtual_pixel_method == UndefinedVirtualPixelMethod))
      y_offset=EdgeY(y_offset,cache_info->rows);
    for (u=0; u < (ssize_t) columns; u+=length)
    {
      ssize_t
        x_offset;

      x_offset=x+u;
      length=(MagickSizeType) MagickMin(cache_info->columns-x_offset,columns-u);
      if (((x_offset < 0) || (x_offset >= (ssize_t) cache_info->columns)) ||
          ((y_offset < 0) || (y_offset >= (ssize_t) cache_info->rows)) ||
          (length == 0))
        {
          MagickModulo
            x_modulo,
            y_modulo;

          /*
            Transfer a single pixel.
          */
          length=(MagickSizeType) 1;
          switch (virtual_pixel_method)
          {
            case EdgeVirtualPixelMethod:
            default:
            {
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                EdgeX(x_offset,cache_info->columns),
                EdgeY(y_offset,cache_info->rows),1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case RandomVirtualPixelMethod:
            {
              if (cache_info->random_info == (RandomInfo *) NULL)
                cache_info->random_info=AcquireRandomInfo();
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                RandomX(cache_info->random_info,cache_info->columns),
                RandomY(cache_info->random_info,cache_info->rows),1UL,1UL,
                *virtual_nexus,exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case DitherVirtualPixelMethod:
            {
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                DitherX(x_offset,cache_info->columns),
                DitherY(y_offset,cache_info->rows),1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case TileVirtualPixelMethod:
            {
              x_modulo=VirtualPixelModulo(x_offset,cache_info->columns);
              y_modulo=VirtualPixelModulo(y_offset,cache_info->rows);
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                x_modulo.remainder,y_modulo.remainder,1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case MirrorVirtualPixelMethod:
            {
              x_modulo=VirtualPixelModulo(x_offset,cache_info->columns);
              if ((x_modulo.quotient & 0x01) == 1L)
                x_modulo.remainder=(ssize_t) cache_info->columns-
                  x_modulo.remainder-1L;
              y_modulo=VirtualPixelModulo(y_offset,cache_info->rows);
              if ((y_modulo.quotient & 0x01) == 1L)
                y_modulo.remainder=(ssize_t) cache_info->rows-
                  y_modulo.remainder-1L;
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                x_modulo.remainder,y_modulo.remainder,1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case HorizontalTileEdgeVirtualPixelMethod:
            {
              x_modulo=VirtualPixelModulo(x_offset,cache_info->columns);
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                x_modulo.remainder,EdgeY(y_offset,cache_info->rows),1UL,1UL,
                *virtual_nexus,exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case VerticalTileEdgeVirtualPixelMethod:
            {
              y_modulo=VirtualPixelModulo(y_offset,cache_info->rows);
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                EdgeX(x_offset,cache_info->columns),y_modulo.remainder,1UL,1UL,
                *virtual_nexus,exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case BackgroundVirtualPixelMethod:
            case BlackVirtualPixelMethod:
            case GrayVirtualPixelMethod:
            case TransparentVirtualPixelMethod:
            case MaskVirtualPixelMethod:
            case WhiteVirtualPixelMethod:
            {
              p=virtual_pixel;
              r=virtual_metacontent;
              break;
            }
            case CheckerTileVirtualPixelMethod:
            {
              x_modulo=VirtualPixelModulo(x_offset,cache_info->columns);
              y_modulo=VirtualPixelModulo(y_offset,cache_info->rows);
              if (((x_modulo.quotient ^ y_modulo.quotient) & 0x01) != 0L)
                {
                  p=virtual_pixel;
                  r=virtual_metacontent;
                  break;
                }
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                x_modulo.remainder,y_modulo.remainder,1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case HorizontalTileVirtualPixelMethod:
            {
              if ((y_offset < 0) || (y_offset >= (ssize_t) cache_info->rows))
                {
                  p=virtual_pixel;
                  r=virtual_metacontent;
                  break;
                }
              x_modulo=VirtualPixelModulo(x_offset,cache_info->columns);
              y_modulo=VirtualPixelModulo(y_offset,cache_info->rows);
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                x_modulo.remainder,y_modulo.remainder,1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
            case VerticalTileVirtualPixelMethod:
            {
              if ((x_offset < 0) || (x_offset >= (ssize_t) cache_info->columns))
                {
                  p=virtual_pixel;
                  r=virtual_metacontent;
                  break;
                }
              x_modulo=VirtualPixelModulo(x_offset,cache_info->columns);
              y_modulo=VirtualPixelModulo(y_offset,cache_info->rows);
              p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,
                x_modulo.remainder,y_modulo.remainder,1UL,1UL,*virtual_nexus,
                exception);
              r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
              break;
            }
          }
          if (p == (const Quantum *) NULL)
            break;
          (void) memcpy(q,p,(size_t) length*cache_info->number_channels*
            sizeof(*p));
          q+=cache_info->number_channels;
          if ((s != (void *) NULL) && (r != (const void *) NULL))
            {
              (void) memcpy(s,r,(size_t) cache_info->metacontent_extent);
              s+=cache_info->metacontent_extent;
            }
          continue;
        }
      /*
        Transfer a run of pixels.
      */
      p=GetVirtualPixelsFromNexus(image,virtual_pixel_method,x_offset,y_offset,
        (size_t) length,1UL,*virtual_nexus,exception);
      if (p == (const Quantum *) NULL)
        break;
      r=GetVirtualMetacontentFromNexus(cache_info,*virtual_nexus);
      (void) memcpy(q,p,(size_t) length*cache_info->number_channels*sizeof(*p));
      q+=length*cache_info->number_channels;
      if ((r != (void *) NULL) && (s != (const void *) NULL))
        {
          (void) memcpy(s,r,(size_t) length);
          s+=length*cache_info->metacontent_extent;
        }
    }
    if (u < (ssize_t) columns)
      break;
  }
  /*
    Free resources.
  */
  if (virtual_metacontent != (void *) NULL)
    virtual_metacontent=(void *) RelinquishMagickMemory(virtual_metacontent);
  virtual_nexus=DestroyPixelCacheNexus(virtual_nexus,1);
  if (v < (ssize_t) rows)
    return((const Quantum *) NULL);
  return(pixels);
}