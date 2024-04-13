int LibRaw::open_bayer(unsigned char *buffer, unsigned datalen,
                       ushort _raw_width, ushort _raw_height,
                       ushort _left_margin, ushort _top_margin,
                       ushort _right_margin, ushort _bottom_margin,
                       unsigned char procflags, unsigned char bayer_pattern,
                       unsigned unused_bits, unsigned otherflags,
                       unsigned black_level)
{
  // this stream will close on recycle()
  if (!buffer || buffer == (void *)-1)
    return LIBRAW_IO_ERROR;

  LibRaw_buffer_datastream *stream;
  try
  {
    stream = new LibRaw_buffer_datastream(buffer, datalen);
  }
  catch (std::bad_alloc)
  {
    recycle();
    return LIBRAW_UNSUFFICIENT_MEMORY;
  }
  if (!stream->valid())
  {
    delete stream;
    return LIBRAW_IO_ERROR;
  }
  ID.input = stream;
  SET_PROC_FLAG(LIBRAW_PROGRESS_OPEN);
  // From identify
  initdata();
  strcpy(imgdata.idata.make, "BayerDump");
  snprintf(imgdata.idata.model, sizeof(imgdata.idata.model) - 1,
           "%u x %u pixels", _raw_width, _raw_height);
  S.flip = procflags >> 2;
  libraw_internal_data.internal_output_params.zero_is_bad = procflags & 2;
  libraw_internal_data.unpacker_data.data_offset = 0;
  S.raw_width = _raw_width;
  S.raw_height = _raw_height;
  S.left_margin = _left_margin;
  S.top_margin = _top_margin;
  S.width = S.raw_width - S.left_margin - _right_margin;
  S.height = S.raw_height - S.top_margin - _bottom_margin;

  imgdata.idata.filters = 0x1010101 * bayer_pattern;
  imgdata.idata.colors =
      4 - !((imgdata.idata.filters & imgdata.idata.filters >> 1) & 0x5555);
  libraw_internal_data.unpacker_data.load_flags = otherflags;
  switch (libraw_internal_data.unpacker_data.tiff_bps =
              (datalen)*8 / (S.raw_width * S.raw_height))
  {
  case 8:
    load_raw = &LibRaw::eight_bit_load_raw;
    break;
  case 10:
    if ((datalen) / S.raw_height * 3 >= S.raw_width * 4)
    {
      load_raw = &LibRaw::android_loose_load_raw;
      break;
    }
    else if (libraw_internal_data.unpacker_data.load_flags & 1)
    {
      load_raw = &LibRaw::android_tight_load_raw;
      break;
    }
  case 12:
    libraw_internal_data.unpacker_data.load_flags |= 128;
    load_raw = &LibRaw::packed_load_raw;
    break;
  case 16:
    libraw_internal_data.unpacker_data.order =
        0x4949 | 0x404 * (libraw_internal_data.unpacker_data.load_flags & 1);
    libraw_internal_data.unpacker_data.tiff_bps -=
        libraw_internal_data.unpacker_data.load_flags >> 4;
    libraw_internal_data.unpacker_data.tiff_bps -=
        libraw_internal_data.unpacker_data.load_flags =
            libraw_internal_data.unpacker_data.load_flags >> 1 & 7;
    load_raw = &LibRaw::unpacked_load_raw;
  }
  C.maximum =
      (1 << libraw_internal_data.unpacker_data.tiff_bps) - (1 << unused_bits);
  C.black = black_level;
  S.iwidth = S.width;
  S.iheight = S.height;
  imgdata.idata.colors = 3;
  imgdata.idata.filters |= ((imgdata.idata.filters >> 2 & 0x22222222) |
                            (imgdata.idata.filters << 2 & 0x88888888)) &
                           imgdata.idata.filters << 1;

  imgdata.idata.raw_count = 1;
  for (int i = 0; i < 4; i++)
    imgdata.color.pre_mul[i] = 1.0;

  strcpy(imgdata.idata.cdesc, "RGBG");

  ID.input_internal = 1;
  SET_PROC_FLAG(LIBRAW_PROGRESS_IDENTIFY);
  return LIBRAW_SUCCESS;
}