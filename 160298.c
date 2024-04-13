void LibRaw::phase_one_load_raw()
{
  int a, b, i;
  ushort akey, bkey, t_mask;

  fseek(ifp, ph1.key_off, SEEK_SET);
  akey = get2();
  bkey = get2();
  t_mask = ph1.format == 1 ? 0x5555 : 0x1354;
  if (ph1.black_col || ph1.black_row)
  {
    imgdata.rawdata.ph1_cblack =
        (short(*)[2])calloc(raw_height * 2, sizeof(ushort));
    merror(imgdata.rawdata.ph1_cblack, "phase_one_load_raw()");
    imgdata.rawdata.ph1_rblack =
        (short(*)[2])calloc(raw_width * 2, sizeof(ushort));
    merror(imgdata.rawdata.ph1_rblack, "phase_one_load_raw()");
    if (ph1.black_col)
    {
      fseek(ifp, ph1.black_col, SEEK_SET);
      read_shorts((ushort *)imgdata.rawdata.ph1_cblack[0], raw_height * 2);
    }
    if (ph1.black_row)
    {
      fseek(ifp, ph1.black_row, SEEK_SET);
      read_shorts((ushort *)imgdata.rawdata.ph1_rblack[0], raw_width * 2);
    }
  }
  fseek(ifp, data_offset, SEEK_SET);
  read_shorts(raw_image, raw_width * raw_height);
  if (ph1.format)
    for (i = 0; i < raw_width * raw_height; i += 2)
    {
      a = raw_image[i + 0] ^ akey;
      b = raw_image[i + 1] ^ bkey;
      raw_image[i + 0] = (a & t_mask) | (b & ~t_mask);
      raw_image[i + 1] = (b & t_mask) | (a & ~t_mask);
    }
}