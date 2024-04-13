void LibRaw::fuji_decode_strip(const struct fuji_compressed_params *info_common,
                               int cur_block, INT64 raw_offset, unsigned dsize)
{
  int cur_block_width, cur_line;
  unsigned line_size;
  struct fuji_compressed_block info;

  init_fuji_block(&info, info_common, raw_offset, dsize);
  line_size = sizeof(ushort) * (info_common->line_width + 2);

  cur_block_width = libraw_internal_data.unpacker_data.fuji_block_width;
  if (cur_block + 1 == libraw_internal_data.unpacker_data.fuji_total_blocks)
  {
    cur_block_width =
        imgdata.sizes.raw_width -
        (libraw_internal_data.unpacker_data.fuji_block_width * cur_block);
    /* Old code, may get incorrect results on GFX50, but luckily large optical
    black cur_block_width = imgdata.sizes.raw_width %
    libraw_internal_data.unpacker_data.fuji_block_width;
    */
  }

  struct i_pair
  {
    int a, b;
  };
  const i_pair mtable[6] = {{_R0, _R3}, {_R1, _R4}, {_G0, _G6},
                            {_G1, _G7}, {_B0, _B3}, {_B1, _B4}},
               ztable[3] = {{_R2, 3}, {_G2, 6}, {_B2, 3}};
  for (cur_line = 0;
       cur_line < libraw_internal_data.unpacker_data.fuji_total_lines;
       cur_line++)
  {
    if (libraw_internal_data.unpacker_data.fuji_raw_type == 16)
      xtrans_decode_block(&info, info_common, cur_line);
    else
      fuji_bayer_decode_block(&info, info_common, cur_line);

    // copy data from line buffers and advance
    for (int i = 0; i < 6; i++)
      memcpy(info.linebuf[mtable[i].a], info.linebuf[mtable[i].b], line_size);

    if (libraw_internal_data.unpacker_data.fuji_raw_type == 16)
      copy_line_to_xtrans(&info, cur_line, cur_block, cur_block_width);
    else
      copy_line_to_bayer(&info, cur_line, cur_block, cur_block_width);

    for (int i = 0; i < 3; i++)
    {
      memset(info.linebuf[ztable[i].a], 0, ztable[i].b * line_size);
      info.linebuf[ztable[i].a][0] = info.linebuf[ztable[i].a - 1][1];
      info.linebuf[ztable[i].a][info_common->line_width + 1] =
          info.linebuf[ztable[i].a - 1][info_common->line_width];
    }
  }

  // release data
  free(info.linealloc);
  free(info.cur_buf);
}