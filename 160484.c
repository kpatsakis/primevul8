void LibRaw::nikon_load_padded_packed_raw() // 12 bit per pixel, padded to 16
                                            // bytes
{
  // libraw_internal_data.unpacker_data.load_flags -> row byte count
  if (libraw_internal_data.unpacker_data.load_flags < 2000 ||
      libraw_internal_data.unpacker_data.load_flags > 64000)
    return;
  unsigned char *buf =
      (unsigned char *)malloc(libraw_internal_data.unpacker_data.load_flags);
  for (int row = 0; row < S.raw_height; row++)
  {
    checkCancel();
    libraw_internal_data.internal_data.input->read(
        buf, libraw_internal_data.unpacker_data.load_flags, 1);
    for (int icol = 0; icol < S.raw_width / 2; icol++)
    {
      imgdata.rawdata.raw_image[(row)*S.raw_width + (icol * 2)] =
          ((buf[icol * 3 + 1] & 0xf) << 8) | buf[icol * 3];
      imgdata.rawdata.raw_image[(row)*S.raw_width + (icol * 2 + 1)] =
          buf[icol * 3 + 2] << 4 | ((buf[icol * 3 + 1] & 0xf0) >> 4);
    }
  }
  free(buf);
}