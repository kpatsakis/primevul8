void LibRaw::deflate_dng_load_raw()
{
  int iifd = find_ifd_by_offset(libraw_internal_data.unpacker_data.data_offset);
  if(iifd < 0 || iifd > libraw_internal_data.identify_data.tiff_nifds)
      throw LIBRAW_EXCEPTION_DECODE_RAW;
  struct tiff_ifd_t *ifd = &tiff_ifd[iifd];

  float *float_raw_image = 0;
  float max = 0.f;

  if (ifd->samples != 1 && ifd->samples != 3 && ifd->samples != 4)
    throw LIBRAW_EXCEPTION_DECODE_RAW; // Only float deflated supported

  if (libraw_internal_data.unpacker_data.tiff_samples != ifd->samples)
    throw LIBRAW_EXCEPTION_DECODE_RAW; // Wrong IFD

  size_t tilesH = (imgdata.sizes.raw_width +
                   libraw_internal_data.unpacker_data.tile_width - 1) /
                  libraw_internal_data.unpacker_data.tile_width;
  size_t tilesV = (imgdata.sizes.raw_height +
                   libraw_internal_data.unpacker_data.tile_length - 1) /
                  libraw_internal_data.unpacker_data.tile_length;
  size_t tileCnt = tilesH * tilesV;

  if (ifd->sample_format == 3)
  { // Floating point data
    float_raw_image = (float *)calloc(
        tileCnt * libraw_internal_data.unpacker_data.tile_length *
            libraw_internal_data.unpacker_data.tile_width * ifd->samples,
        sizeof(float));
    // imgdata.color.maximum = 65535;
    // imgdata.color.black = 0;
    // memset(imgdata.color.cblack,0,sizeof(imgdata.color.cblack));
  }
  else
    throw LIBRAW_EXCEPTION_DECODE_RAW; // Only float deflated supported

  int xFactor;
  switch (ifd->predictor)
  {
  case 3:
  default:
    xFactor = 1;
    break;
  case 34894:
    xFactor = 2;
    break;
  case 34895:
    xFactor = 4;
    break;
  }

  if (libraw_internal_data.unpacker_data.tile_length < INT_MAX)
  {
    if (tileCnt < 1 || tileCnt > 1000000)
      throw LIBRAW_EXCEPTION_DECODE_RAW;

    size_t *tOffsets = (size_t *)malloc(tileCnt * sizeof(size_t));
    for (int t = 0; t < tileCnt; ++t)
      tOffsets[t] = get4();

    size_t *tBytes = (size_t *)malloc(tileCnt * sizeof(size_t));
    unsigned long maxBytesInTile = 0;
    if (tileCnt == 1)
      tBytes[0] = maxBytesInTile = ifd->bytes;
    else
    {
      libraw_internal_data.internal_data.input->seek(ifd->bytes, SEEK_SET);
      for (size_t t = 0; t < tileCnt; ++t)
      {
        tBytes[t] = get4();
        maxBytesInTile = MAX(maxBytesInTile, tBytes[t]);
      }
    }
    unsigned tilePixels = libraw_internal_data.unpacker_data.tile_width *
                          libraw_internal_data.unpacker_data.tile_length;
    unsigned pixelSize = sizeof(float) * ifd->samples;
    unsigned tileBytes = tilePixels * pixelSize;
    unsigned tileRowBytes =
        libraw_internal_data.unpacker_data.tile_width * pixelSize;

    unsigned char *cBuffer = (unsigned char *)malloc(maxBytesInTile);
    unsigned char *uBuffer = (unsigned char *)malloc(
        tileBytes + tileRowBytes); // extra row for decoding

    for (size_t y = 0, t = 0; y < imgdata.sizes.raw_height;
         y += libraw_internal_data.unpacker_data.tile_length)
    {
      for (size_t x = 0; x < imgdata.sizes.raw_width;
           x += libraw_internal_data.unpacker_data.tile_width, ++t)
      {
        libraw_internal_data.internal_data.input->seek(tOffsets[t], SEEK_SET);
        libraw_internal_data.internal_data.input->read(cBuffer, 1, tBytes[t]);
        unsigned long dstLen = tileBytes;
        int err =
            uncompress(uBuffer + tileRowBytes, &dstLen, cBuffer, tBytes[t]);
        if (err != Z_OK)
        {
          free(tOffsets);
          free(tBytes);
          free(cBuffer);
          free(uBuffer);
          throw LIBRAW_EXCEPTION_DECODE_RAW;
          return;
        }
        else
        {
          int bytesps = ifd->bps >> 3;
          size_t rowsInTile =
              y + libraw_internal_data.unpacker_data.tile_length >
                      imgdata.sizes.raw_height
                  ? imgdata.sizes.raw_height - y
                  : libraw_internal_data.unpacker_data.tile_length;
          size_t colsInTile =
              x + libraw_internal_data.unpacker_data.tile_width >
                      imgdata.sizes.raw_width
                  ? imgdata.sizes.raw_width - x
                  : libraw_internal_data.unpacker_data.tile_width;

          for (size_t row = 0; row < rowsInTile;
               ++row) // do not process full tile if not needed
          {
            unsigned char *dst =
                uBuffer + row * libraw_internal_data.unpacker_data.tile_width *
                              bytesps * ifd->samples;
            unsigned char *src = dst + tileRowBytes;
            DecodeFPDelta(src, dst,
                          libraw_internal_data.unpacker_data.tile_width /
                              xFactor,
                          ifd->samples * xFactor, bytesps);
            float lmax = expandFloats(
                dst,
                libraw_internal_data.unpacker_data.tile_width * ifd->samples,
                bytesps);
            max = MAX(max, lmax);
            unsigned char *dst2 = (unsigned char *)&float_raw_image
                [((y + row) * imgdata.sizes.raw_width + x) * ifd->samples];
            memmove(dst2, dst, colsInTile * ifd->samples * sizeof(float));
          }
        }
      }
    }
    free(tOffsets);
    free(tBytes);
    free(cBuffer);
    free(uBuffer);
  }
  imgdata.color.fmaximum = max;

  // Set fields according to data format

  imgdata.rawdata.raw_alloc = float_raw_image;
  if (ifd->samples == 1)
  {
    imgdata.rawdata.float_image = float_raw_image;
    imgdata.rawdata.sizes.raw_pitch = imgdata.sizes.raw_pitch =
        imgdata.sizes.raw_width * 4;
  }
  else if (ifd->samples == 3)
  {
    imgdata.rawdata.float3_image = (float(*)[3])float_raw_image;
    imgdata.rawdata.sizes.raw_pitch = imgdata.sizes.raw_pitch =
        imgdata.sizes.raw_width * 12;
  }
  else if (ifd->samples == 4)
  {
    imgdata.rawdata.float4_image = (float(*)[4])float_raw_image;
    imgdata.rawdata.sizes.raw_pitch = imgdata.sizes.raw_pitch =
        imgdata.sizes.raw_width * 16;
  }

  if (imgdata.params.raw_processing_options &
      LIBRAW_PROCESSING_CONVERTFLOAT_TO_INT)
    convertFloatToInt(); // with default settings
}