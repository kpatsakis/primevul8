void LibRaw::nikon_load_striped_packed_raw()
{
  int vbits = 0, bwide, rbits, bite, row, col, val, i;

  UINT64 bitbuf = 0;
  unsigned load_flags = 24; // libraw_internal_data.unpacker_data.load_flags;
  unsigned tiff_bps = libraw_internal_data.unpacker_data.tiff_bps;
  int tiff_compress = libraw_internal_data.unpacker_data.tiff_compress;

  struct tiff_ifd_t *ifd = &tiff_ifd[0];
  while (ifd < &tiff_ifd[libraw_internal_data.identify_data.tiff_nifds] &&
         ifd->offset != libraw_internal_data.unpacker_data.data_offset)
    ++ifd;
  if (ifd == &tiff_ifd[libraw_internal_data.identify_data.tiff_nifds])
    throw LIBRAW_EXCEPTION_DECODE_RAW;

  if (!ifd->rows_per_strip || !ifd->strip_offsets_count)
    return; // not unpacked
  int stripcnt = 0;

  bwide = S.raw_width * tiff_bps / 8;
  bwide += bwide & load_flags >> 7;
  rbits = bwide * 8 - S.raw_width * tiff_bps;
  if (load_flags & 1)
    bwide = bwide * 16 / 15;
  bite = 8 + (load_flags & 24);
  for (row = 0; row < S.raw_height; row++)
  {
    checkCancel();
    if (!(row % ifd->rows_per_strip))
    {
      if (stripcnt >= ifd->strip_offsets_count)
        return; // run out of data
      libraw_internal_data.internal_data.input->seek(
          ifd->strip_offsets[stripcnt], SEEK_SET);
      stripcnt++;
    }
    for (col = 0; col < S.raw_width; col++)
    {
      for (vbits -= tiff_bps; vbits < 0; vbits += bite)
      {
        bitbuf <<= bite;
        for (i = 0; i < bite; i += 8)
          bitbuf |=
              (unsigned)(libraw_internal_data.internal_data.input->get_char()
                         << i);
      }
      imgdata.rawdata.raw_image[(row)*S.raw_width + (col)] =
          bitbuf << (64 - tiff_bps - vbits) >> (64 - tiff_bps);
    }
    vbits -= rbits;
  }
}