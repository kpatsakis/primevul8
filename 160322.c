int LibRaw::phase_one_subtract_black(ushort *src, ushort *dest)
{

  try
  {
    if (O.user_black < 0 && O.user_cblack[0] <= -1000000 &&
        O.user_cblack[1] <= -1000000 && O.user_cblack[2] <= -1000000 &&
        O.user_cblack[3] <= -1000000)
    {
      if (!imgdata.rawdata.ph1_cblack || !imgdata.rawdata.ph1_rblack)
      {
        register int bl = imgdata.color.phase_one_data.t_black;
        for (int row = 0; row < S.raw_height; row++)
        {
          checkCancel();
          for (int col = 0; col < S.raw_width; col++)
          {
            int idx = row * S.raw_width + col;
            int val = int(src[idx]) - bl;
            dest[idx] = val > 0 ? val : 0;
          }
        }
      }
      else
      {
        register int bl = imgdata.color.phase_one_data.t_black;
        for (int row = 0; row < S.raw_height; row++)
        {
          checkCancel();
          for (int col = 0; col < S.raw_width; col++)
          {
            int idx = row * S.raw_width + col;
            int val =
                int(src[idx]) - bl +
                imgdata.rawdata
                    .ph1_cblack[row][col >= imgdata.rawdata.color.phase_one_data
                                                .split_col] +
                imgdata.rawdata
                    .ph1_rblack[col][row >= imgdata.rawdata.color.phase_one_data
                                                .split_row];
            dest[idx] = val > 0 ? val : 0;
          }
        }
      }
    }
    else // black set by user interaction
    {
      // Black level in cblack!
      for (int row = 0; row < S.raw_height; row++)
      {
        checkCancel();
        unsigned short cblk[16];
        for (int cc = 0; cc < 16; cc++)
          cblk[cc] = C.cblack[fcol(row, cc)];
        for (int col = 0; col < S.raw_width; col++)
        {
          int idx = row * S.raw_width + col;
          ushort val = src[idx];
          ushort bl = cblk[col & 0xf];
          dest[idx] = val > bl ? val - bl : 0;
        }
      }
    }
    return 0;
  }
  catch (LibRaw_exceptions err)
  {
    return LIBRAW_CANCELLED_BY_CALLBACK;
  }
}