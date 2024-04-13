void LibRaw::copy_line_to_xtrans(struct fuji_compressed_block *info,
                                 int cur_line, int cur_block,
                                 int cur_block_width)
{
  ushort *lineBufB[3];
  ushort *lineBufG[6];
  ushort *lineBufR[3];
  unsigned pixel_count;
  ushort *line_buf;
  int index;

  int offset = libraw_internal_data.unpacker_data.fuji_block_width * cur_block +
               6 * imgdata.sizes.raw_width * cur_line;
  ushort *raw_block_data = imgdata.rawdata.raw_image + offset;
  int row_count = 0;

  for (int i = 0; i < 3; i++)
  {
    lineBufR[i] = info->linebuf[_R2 + i] + 1;
    lineBufB[i] = info->linebuf[_B2 + i] + 1;
  }
  for (int i = 0; i < 6; i++)
    lineBufG[i] = info->linebuf[_G2 + i] + 1;

  while (row_count < 6)
  {
    pixel_count = 0;
    while (pixel_count < cur_block_width)
    {
      switch (imgdata.idata.xtrans_abs[row_count][(pixel_count % 6)])
      {
      case 0: // red
        line_buf = lineBufR[row_count >> 1];
        break;
      case 1:  // green
      default: // to make static analyzer happy
        line_buf = lineBufG[row_count];
        break;
      case 2: // blue
        line_buf = lineBufB[row_count >> 1];
        break;
      }

      index = (((pixel_count * 2 / 3) & 0x7FFFFFFE) | ((pixel_count % 3) & 1)) +
              ((pixel_count % 3) >> 1);
      raw_block_data[pixel_count] = line_buf[index];

      ++pixel_count;
    }
    ++row_count;
    raw_block_data += imgdata.sizes.raw_width;
  }
}