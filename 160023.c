  int FCF(int row, int col)
  {
    int rr, cc;
    if (libraw_internal_data.unpacker_data.fuji_layout)
    {
      rr = libraw_internal_data.internal_output_params.fuji_width - 1 - col +
           (row >> 1);
      cc = col + ((row + 1) >> 1);
    }
    else
    {
      rr = libraw_internal_data.internal_output_params.fuji_width - 1 + row -
           (col >> 1);
      cc = row + ((col + 1) >> 1);
    }
    return FC(rr, cc);
  }