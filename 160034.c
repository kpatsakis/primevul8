void LibRaw::panasonicC7_load_raw()
{
  const int rowstep = 16;
  int pixperblock = libraw_internal_data.unpacker_data.pana_bpp == 14 ? 9 : 10;
  int rowbytes = imgdata.sizes.raw_width / pixperblock * 16;
  unsigned char *iobuf = (unsigned char *)malloc(rowbytes * rowstep);
  merror(iobuf, "panasonicC7_load_raw()");
  for (int row = 0; row < imgdata.sizes.raw_height - rowstep + 1;
       row += rowstep)
  {
    int rowstoread = MIN(rowstep, imgdata.sizes.raw_height - row);
    if (libraw_internal_data.internal_data.input->read(
            iobuf, rowbytes, rowstoread) != rowstoread)
      throw LIBRAW_EXCEPTION_IO_EOF;
    unsigned char *bytes = iobuf;
    for (int crow = 0; crow < rowstoread; crow++)
    {
      unsigned short *rowptr =
          &imgdata.rawdata
               .raw_image[(row + crow) * imgdata.sizes.raw_pitch / 2];
      for (int col = 0; col < imgdata.sizes.raw_width - pixperblock + 1;
           col += pixperblock, bytes += 16)
      {
        if (libraw_internal_data.unpacker_data.pana_bpp == 14)
        {
          rowptr[col] = bytes[0] + ((bytes[1] & 0x3F) << 8);
          rowptr[col + 1] =
              (bytes[1] >> 6) + 4 * (bytes[2]) + ((bytes[3] & 0xF) << 10);
          rowptr[col + 2] =
              (bytes[3] >> 4) + 16 * (bytes[4]) + ((bytes[5] & 3) << 12);
          rowptr[col + 3] = ((bytes[5] & 0xFC) >> 2) + (bytes[6] << 6);
          rowptr[col + 4] = bytes[7] + ((bytes[8] & 0x3F) << 8);
          rowptr[col + 5] =
              (bytes[8] >> 6) + 4 * bytes[9] + ((bytes[10] & 0xF) << 10);
          rowptr[col + 6] =
              (bytes[10] >> 4) + 16 * bytes[11] + ((bytes[12] & 3) << 12);
          rowptr[col + 7] = ((bytes[12] & 0xFC) >> 2) + (bytes[13] << 6);
          rowptr[col + 8] = bytes[14] + ((bytes[15] & 0x3F) << 8);
        }
        else if (libraw_internal_data.unpacker_data.pana_bpp ==
                 12) // have not seen in the wild yet
        {
          rowptr[col] = ((bytes[1] & 0xF) << 8) + bytes[0];
          rowptr[col + 1] = 16 * bytes[2] + (bytes[1] >> 4);
          rowptr[col + 2] = ((bytes[4] & 0xF) << 8) + bytes[3];
          rowptr[col + 3] = 16 * bytes[5] + (bytes[4] >> 4);
          rowptr[col + 4] = ((bytes[7] & 0xF) << 8) + bytes[6];
          rowptr[col + 5] = 16 * bytes[8] + (bytes[7] >> 4);
          rowptr[col + 6] = ((bytes[10] & 0xF) << 8) + bytes[9];
          rowptr[col + 7] = 16 * bytes[11] + (bytes[10] >> 4);
          rowptr[col + 8] = ((bytes[13] & 0xF) << 8) + bytes[12];
          rowptr[col + 9] = 16 * bytes[14] + (bytes[13] >> 4);
        }
      }
    }
  }
  free(iobuf);
}