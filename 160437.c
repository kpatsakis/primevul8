void LibRaw::init_fuji_compr(struct fuji_compressed_params *info)
{
  int cur_val, i;
  int8_t *qt;

  if ((libraw_internal_data.unpacker_data.fuji_block_width % 3 &&
       libraw_internal_data.unpacker_data.fuji_raw_type == 16) ||
      (libraw_internal_data.unpacker_data.fuji_block_width & 1 &&
       libraw_internal_data.unpacker_data.fuji_raw_type == 0))
    derror();

  info->q_table =
      (int8_t *)malloc(2 << libraw_internal_data.unpacker_data.fuji_bits);
  merror(info->q_table, "init_fuji_compr()");

  if (libraw_internal_data.unpacker_data.fuji_raw_type == 16)
    info->line_width =
        (libraw_internal_data.unpacker_data.fuji_block_width * 2) / 3;
  else
    info->line_width = libraw_internal_data.unpacker_data.fuji_block_width >> 1;

  info->q_point[0] = 0;
  info->q_point[1] = 0x12;
  info->q_point[2] = 0x43;
  info->q_point[3] = 0x114;
  info->q_point[4] = (1 << libraw_internal_data.unpacker_data.fuji_bits) - 1;
  info->min_value = 0x40;

  cur_val = -info->q_point[4];
  for (qt = info->q_table; cur_val <= info->q_point[4]; ++qt, ++cur_val)
  {
    if (cur_val <= -info->q_point[3])
      *qt = -4;
    else if (cur_val <= -info->q_point[2])
      *qt = -3;
    else if (cur_val <= -info->q_point[1])
      *qt = -2;
    else if (cur_val < 0)
      *qt = -1;
    else if (cur_val == 0)
      *qt = 0;
    else if (cur_val < info->q_point[1])
      *qt = 1;
    else if (cur_val < info->q_point[2])
      *qt = 2;
    else if (cur_val < info->q_point[3])
      *qt = 3;
    else
      *qt = 4;
  }

  // populting gradients
  info->total_values = (1 << libraw_internal_data.unpacker_data.fuji_bits);
  info->raw_bits = libraw_internal_data.unpacker_data.fuji_bits;
  info->max_bits = 4 * info->raw_bits;
  info->maxDiff = info->total_values >> 6;
}