void LibRaw::fuji_bayer_decode_block(
    struct fuji_compressed_block *info,
    const struct fuji_compressed_params *params, int cur_line)
{
  int r_even_pos = 0, r_odd_pos = 1;
  int g_even_pos = 0, g_odd_pos = 1;
  int b_even_pos = 0, b_odd_pos = 1;

  int errcnt = 0;

  const int line_width = params->line_width;

  while (g_even_pos < line_width || g_odd_pos < line_width)
  {
    if (g_even_pos < line_width)
    {
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_R2] + 1,
                                        r_even_pos, info->grad_even[0]);
      r_even_pos += 2;
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_G2] + 1,
                                        g_even_pos, info->grad_even[0]);
      g_even_pos += 2;
    }
    if (g_even_pos > 8)
    {
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_R2] + 1,
                                       r_odd_pos, info->grad_odd[0]);
      r_odd_pos += 2;
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_G2] + 1,
                                       g_odd_pos, info->grad_odd[0]);
      g_odd_pos += 2;
    }
  }

  fuji_extend_red(info->linebuf, line_width);
  fuji_extend_green(info->linebuf, line_width);

  g_even_pos = 0, g_odd_pos = 1;

  while (g_even_pos < line_width || g_odd_pos < line_width)
  {
    if (g_even_pos < line_width)
    {
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_G3] + 1,
                                        g_even_pos, info->grad_even[1]);
      g_even_pos += 2;
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_B2] + 1,
                                        b_even_pos, info->grad_even[1]);
      b_even_pos += 2;
    }
    if (g_even_pos > 8)
    {
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_G3] + 1,
                                       g_odd_pos, info->grad_odd[1]);
      g_odd_pos += 2;
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_B2] + 1,
                                       b_odd_pos, info->grad_odd[1]);
      b_odd_pos += 2;
    }
  }

  fuji_extend_green(info->linebuf, line_width);
  fuji_extend_blue(info->linebuf, line_width);

  r_even_pos = 0, r_odd_pos = 1;
  g_even_pos = 0, g_odd_pos = 1;

  while (g_even_pos < line_width || g_odd_pos < line_width)
  {
    if (g_even_pos < line_width)
    {
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_R3] + 1,
                                        r_even_pos, info->grad_even[2]);
      r_even_pos += 2;
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_G4] + 1,
                                        g_even_pos, info->grad_even[2]);
      g_even_pos += 2;
    }
    if (g_even_pos > 8)
    {
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_R3] + 1,
                                       r_odd_pos, info->grad_odd[2]);
      r_odd_pos += 2;
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_G4] + 1,
                                       g_odd_pos, info->grad_odd[2]);
      g_odd_pos += 2;
    }
  }

  fuji_extend_red(info->linebuf, line_width);
  fuji_extend_green(info->linebuf, line_width);

  g_even_pos = 0, g_odd_pos = 1;
  b_even_pos = 0, b_odd_pos = 1;

  while (g_even_pos < line_width || g_odd_pos < line_width)
  {
    if (g_even_pos < line_width)
    {
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_G5] + 1,
                                        g_even_pos, info->grad_even[0]);
      g_even_pos += 2;
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_B3] + 1,
                                        b_even_pos, info->grad_even[0]);
      b_even_pos += 2;
    }
    if (g_even_pos > 8)
    {
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_G5] + 1,
                                       g_odd_pos, info->grad_odd[0]);
      g_odd_pos += 2;
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_B3] + 1,
                                       b_odd_pos, info->grad_odd[0]);
      b_odd_pos += 2;
    }
  }

  fuji_extend_green(info->linebuf, line_width);
  fuji_extend_blue(info->linebuf, line_width);

  r_even_pos = 0, r_odd_pos = 1;
  g_even_pos = 0, g_odd_pos = 1;

  while (g_even_pos < line_width || g_odd_pos < line_width)
  {
    if (g_even_pos < line_width)
    {
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_R4] + 1,
                                        r_even_pos, info->grad_even[1]);
      r_even_pos += 2;
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_G6] + 1,
                                        g_even_pos, info->grad_even[1]);
      g_even_pos += 2;
    }
    if (g_even_pos > 8)
    {
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_R4] + 1,
                                       r_odd_pos, info->grad_odd[1]);
      r_odd_pos += 2;
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_G6] + 1,
                                       g_odd_pos, info->grad_odd[1]);
      g_odd_pos += 2;
    }
  }

  fuji_extend_red(info->linebuf, line_width);
  fuji_extend_green(info->linebuf, line_width);

  g_even_pos = 0, g_odd_pos = 1;
  b_even_pos = 0, b_odd_pos = 1;

  while (g_even_pos < line_width || g_odd_pos < line_width)
  {
    if (g_even_pos < line_width)
    {
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_G7] + 1,
                                        g_even_pos, info->grad_even[2]);
      g_even_pos += 2;
      errcnt += fuji_decode_sample_even(info, params, info->linebuf[_B4] + 1,
                                        b_even_pos, info->grad_even[2]);
      b_even_pos += 2;
    }
    if (g_even_pos > 8)
    {
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_G7] + 1,
                                       g_odd_pos, info->grad_odd[2]);
      g_odd_pos += 2;
      errcnt += fuji_decode_sample_odd(info, params, info->linebuf[_B4] + 1,
                                       b_odd_pos, info->grad_odd[2]);
      b_odd_pos += 2;
    }
  }

  fuji_extend_green(info->linebuf, line_width);
  fuji_extend_blue(info->linebuf, line_width);

  if (errcnt)
    derror();
}