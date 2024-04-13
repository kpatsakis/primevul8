void LibRaw::parseOlympus_ImageProcessing(unsigned tag, unsigned type,
                                          unsigned len, unsigned dng_writer)
{
  // uptag 0x2040

  int i, c, wb[4], nWB, tWB, wbG;
  ushort CT;
  short sorder;

  if ((tag == 0x0100) && (dng_writer == nonDNG))
  {
    cam_mul[0] = get2() / 256.0;
    cam_mul[2] = get2() / 256.0;
  }
  else if ((tag == 0x0101) && (len == 2) &&
           (!strncasecmp(model, "E-410", 5) || !strncasecmp(model, "E-510", 5)))
  {
    for (i = 0; i < 64; i++)
    {
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] =
          imgdata.color.WB_Coeffs[i][1] = imgdata.color.WB_Coeffs[i][3] = 0x100;
    }
    for (i = 64; i < 256; i++)
    {
      imgdata.color.WB_Coeffs[i][1] = imgdata.color.WB_Coeffs[i][3] = 0x100;
    }
  }
  else if ((tag > 0x0101) && (tag <= 0x0111))
  {
    nWB = tag - 0x0101;
    tWB = Oly_wb_list2[nWB << 1];
    CT = Oly_wb_list2[(nWB << 1) | 1];
    wb[0] = get2();
    wb[2] = get2();
    if (tWB != 0x100)
    {
      imgdata.color.WB_Coeffs[tWB][0] = wb[0];
      imgdata.color.WB_Coeffs[tWB][2] = wb[2];
    }
    if (CT)
    {
      imgdata.color.WBCT_Coeffs[nWB - 1][0] = CT;
      imgdata.color.WBCT_Coeffs[nWB - 1][1] = wb[0];
      imgdata.color.WBCT_Coeffs[nWB - 1][3] = wb[2];
    }
    if (len == 4)
    {
      wb[1] = get2();
      wb[3] = get2();
      if (tWB != 0x100)
      {
        imgdata.color.WB_Coeffs[tWB][1] = wb[1];
        imgdata.color.WB_Coeffs[tWB][3] = wb[3];
      }
      if (CT)
      {
        imgdata.color.WBCT_Coeffs[nWB - 1][2] = wb[1];
        imgdata.color.WBCT_Coeffs[nWB - 1][4] = wb[3];
      }
    }
  }
  else if ((tag >= 0x0112) && (tag <= 0x011e))
  {
    nWB = tag - 0x0112;
    wbG = get2();
    tWB = Oly_wb_list2[nWB << 1];
    if (nWB)
      imgdata.color.WBCT_Coeffs[nWB - 1][2] =
          imgdata.color.WBCT_Coeffs[nWB - 1][4] = wbG;
    if (tWB != 0x100)
      imgdata.color.WB_Coeffs[tWB][1] = imgdata.color.WB_Coeffs[tWB][3] = wbG;
  }
  else if (tag == 0x011f)
  {
    wbG = get2();
    if (imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][0])
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][1] =
          imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][3] = wbG;
    FORC4 if (imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + c][0])
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + c][1] =
        imgdata.color.WB_Coeffs[LIBRAW_WBI_Custom1 + c][3] = wbG;
  }
  else if (tag == 0x0121)
  {
    imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][0] = get2();
    imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][2] = get2();
    if (len == 4)
    {
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][1] = get2();
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Flash][3] = get2();
    }
  }
  else if ((tag == 0x0200) && (dng_writer == nonDNG) &&
           strcmp(software, "v757-71"))
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
  else if ((tag == 0x0805) && (len == 2))
  {
    imOly.SensorCalibration[0] = getreal(type);
    imOly.SensorCalibration[1] = getreal(type);
    if (dng_writer == nonDNG)
      FORC4 imgdata.color.linear_max[c] = imOly.SensorCalibration[0];
  }
  else if (tag == 0x1112)
  {
    sorder = order;
    order = 0x4d4d;
    c = get2();
    order = sorder;
    switch (c)
    {
    case 0x21:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_3to2;
      break;
    case 0x31:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_16to9;
      break;
    case 0x41:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_1to1;
      break;
    case 0x91:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_4to3;
      break;
    }
  }
  else if (tag == 0x1113)
  {
    imgdata.sizes.raw_inset_crop.cleft = get2();
    imgdata.sizes.raw_inset_crop.ctop = get2();
    imgdata.sizes.raw_inset_crop.cwidth =
        get2() - imgdata.sizes.raw_inset_crop.cleft;
    imgdata.sizes.raw_inset_crop.cheight =
        get2() - imgdata.sizes.raw_inset_crop.ctop;
  }
  else if (tag == 0x1306)
  {
    c = get2();
    if ((c != 0) && (c != 100))
    {
      if (c < 61)
        imgdata.makernotes.common.CameraTemperature = (float)c;
      else
        imgdata.makernotes.common.CameraTemperature = (float)(c - 32) / 1.8f;
      if ((imgdata.makernotes.common.exifAmbientTemperature > -273.15f) &&
          ((OlyID == 0x4434353933ULL) || // TG-5
           (OlyID == 0x4434363033ULL))   // TG-6
      )
        imgdata.makernotes.common.CameraTemperature += imgdata.makernotes.common.exifAmbientTemperature;
    }
  }

  return;
}