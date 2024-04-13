void LibRaw::init_fuji_block(struct fuji_compressed_block *info,
                             const struct fuji_compressed_params *params,
                             INT64 raw_offset, unsigned dsize)
{
  info->linealloc =
      (ushort *)calloc(sizeof(ushort), _ltotal * (params->line_width + 2));
  merror(info->linealloc, "init_fuji_block()");

  INT64 fsize = libraw_internal_data.internal_data.input->size();
  info->max_read_size =
      _min(unsigned(fsize - raw_offset), dsize); // Data size may be incorrect?
  info->fillbytes = 1;

  info->input = libraw_internal_data.internal_data.input;
  info->linebuf[_R0] = info->linealloc;
  for (int i = _R1; i <= _B4; i++)
    info->linebuf[i] = info->linebuf[i - 1] + params->line_width + 2;

  // init buffer
  info->cur_buf = (uchar *)malloc(XTRANS_BUF_SIZE);
  merror(info->cur_buf, "init_fuji_block()");
  info->cur_bit = 0;
  info->cur_pos = 0;
  info->cur_buf_offset = raw_offset;
  for (int j = 0; j < 3; j++)
    for (int i = 0; i < 41; i++)
    {
      info->grad_even[j][i].value1 = params->maxDiff;
      info->grad_even[j][i].value2 = 1;
      info->grad_odd[j][i].value1 = params->maxDiff;
      info->grad_odd[j][i].value2 = 1;
    }

  info->cur_buf_size = 0;
  fuji_fill_buffer(info);
}