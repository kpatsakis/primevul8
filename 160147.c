void LibRaw::crop_masked_pixels()
{
  int row, col;
  unsigned c, m, zero, val;
#define mblack imgdata.color.black_stat

  if (mask[0][3] > 0)
    goto mask_set;
  if (load_raw == &LibRaw::canon_load_raw ||
      load_raw == &LibRaw::lossless_jpeg_load_raw ||
      load_raw == &LibRaw::crxLoadRaw)
  {
    mask[0][1] = mask[1][1] += 2;
    mask[0][3] -= 2;
    goto sides;
  }
  if (load_raw == &LibRaw::canon_600_load_raw ||
      load_raw == &LibRaw::sony_load_raw ||
      (load_raw == &LibRaw::eight_bit_load_raw && strncmp(model, "DC2", 3)) ||
      load_raw == &LibRaw::kodak_262_load_raw ||
      (load_raw == &LibRaw::packed_load_raw && (load_flags & 32)))
  {
  sides:
    mask[0][0] = mask[1][0] = top_margin;
    mask[0][2] = mask[1][2] = top_margin + height;
    mask[0][3] += left_margin;
    mask[1][1] += left_margin + width;
    mask[1][3] += raw_width;
  }
  if (load_raw == &LibRaw::nokia_load_raw)
  {
    mask[0][2] = top_margin;
    mask[0][3] = width;
  }
  if (load_raw == &LibRaw::broadcom_load_raw)
  {
    mask[0][2] = top_margin;
    mask[0][3] = width;
  }
mask_set:
  memset(mblack, 0, sizeof mblack);
  for (zero = m = 0; m < 8; m++)
    for (row = MAX(mask[m][0], 0); row < MIN(mask[m][2], raw_height); row++)
      for (col = MAX(mask[m][1], 0); col < MIN(mask[m][3], raw_width); col++)
      {
        c = FC(row - top_margin, col - left_margin);
        mblack[c] += val = raw_image[(row)*raw_pitch / 2 + (col)];
        mblack[4 + c]++;
        zero += !val;
      }
  if (load_raw == &LibRaw::canon_600_load_raw && width < raw_width)
  {
    black = (mblack[0] + mblack[1] + mblack[2] + mblack[3]) /
                MAX(1, (mblack[4] + mblack[5] + mblack[6] + mblack[7])) -
            4;
    canon_600_correct();
  }
  else if (zero < mblack[4] && mblack[5] && mblack[6] && mblack[7])
  {
    FORC4 cblack[c] = mblack[c] / MAX(1, mblack[4 + c]);
    black = cblack[4] = cblack[5] = cblack[6] = 0;
  }
}