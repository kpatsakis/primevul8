void LibRaw::parseOlympus_RawInfo(unsigned tag, unsigned type, unsigned len,
                                  unsigned dng_writer)
{
  // uptag 0x3000

  int wb_ind, c, i;

  if ((tag == 0x0110) && strcmp(software, "v757-71"))
  {
    imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][0] = get2();
    imgdata.color.WB_Coeffs[LIBRAW_WBI_Auto][2] = get2();
    if (len == 2)
    {
      for (i = 0; i < 256; i++)
        imgdata.color.WB_Coeffs[i][1] = imgdata.color.WB_Coeffs[i][3] = 0x100;
    }
  }
  else if ((((tag >= 0x0120) && (tag <= 0x0124)) ||
            ((tag >= 0x0130) && (tag <= 0x0133))) &&
           strcmp(software, "v757-71"))
  {
    if (tag <= 0x0124)
      wb_ind = tag - 0x0120;
    else
      wb_ind = tag - 0x0130 + 5;

    imgdata.color.WB_Coeffs[Oly_wb_list1[wb_ind]][0] = get2();
    imgdata.color.WB_Coeffs[Oly_wb_list1[wb_ind]][2] = get2();
  }
  else if ((tag == 0x0200) && (dng_writer == nonDNG))
  {
    for (i = 0; i < 3; i++)
    {
      if (!imOly.ColorSpace)
      {
        FORC3 cmatrix[i][c] = ((short)get2()) / 256.0;
      }
      else
      {
        FORC3 imgdata.color.ccm[i][c] = ((short)get2()) / 256.0;
      }
    }
  }
  else if ((tag == 0x0600) && (dng_writer == nonDNG))
  {
    FORC4 cblack[c ^ c >> 1] = get2();
  }
  else if ((tag == 0x0612) && (dng_writer == nonDNG))
  {
    imgdata.sizes.raw_inset_crop.cleft = get2();
  }
  else if ((tag == 0x0613) && (dng_writer == nonDNG))
  {
    imgdata.sizes.raw_inset_crop.ctop = get2();
  }
  else if ((tag == 0x0614) && (dng_writer == nonDNG))
  {
    imgdata.sizes.raw_inset_crop.cwidth = get2();
  }
  else if ((tag == 0x0615) && (dng_writer == nonDNG))
  {
    imgdata.sizes.raw_inset_crop.cheight = get2();
  }
  return;
}