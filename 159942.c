void LibRaw::copy_line_to_bayer(struct fuji_compressed_block *info,
                                int cur_line, int cur_block,
                                int cur_block_width)
{
  ushort *lineBufB[3];
  ushort *lineBufG[6];
  ushort *lineBufR[3];
  unsigned pixel_count;
  ushort *line_buf;

  int fuji_bayer[2][2];
  for (int r = 0; r < 2; r++)
    for (int c = 0; c < 2; c++)
      fuji_bayer[r][c] = FC(r, c); // We'll downgrade G2 to G below

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
      switch (fuji_bayer[row_count & 1][pixel_count & 1])
      {
      case 0: // red
        line_buf = lineBufR[row_count >> 1];
        break;
      case 1:  // green
      case 3:  // second green
      default: // to make static analyzer happy
        line_buf = lineBufG[row_count];
        break;
      case 2: // blue
        line_buf = lineBufB[row_count >> 1];
        break;
      }

      raw_block_data[pixel_count] = line_buf[pixel_count >> 1];
      ++pixel_count;
    }
    ++row_count;
    raw_block_data += imgdata.sizes.raw_width;
  }
}