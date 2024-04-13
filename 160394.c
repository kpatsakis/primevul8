void LibRaw::Canon_WBCTpresets(short WBCTversion)
{

  int i;
  float norm;

  if (WBCTversion == 0)
  { // tint, as shot R, as shot B, CСT
    for (i = 0; i < 15; i++)
    {
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] = 1.0f;
      fseek(ifp, 2, SEEK_CUR);
      imgdata.color.WBCT_Coeffs[i][1] = 1024.0f / fMAX(get2(), 1.f);
      imgdata.color.WBCT_Coeffs[i][3] = 1024.0f / fMAX(get2(), 1.f);
      imgdata.color.WBCT_Coeffs[i][0] = get2();
    }
  }
  else if (WBCTversion == 1)
  { // as shot R, as shot B, tint, CСT
    for (i = 0; i < 15; i++)
    {
      imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] = 1.0f;
      imgdata.color.WBCT_Coeffs[i][1] = 1024.0f / fMAX(get2(), 1.f);
      imgdata.color.WBCT_Coeffs[i][3] = 1024.0f / fMAX(get2(), 1.f);
      fseek(ifp, 2, SEEK_CUR);
      imgdata.color.WBCT_Coeffs[i][0] = get2();
    }
  }
  else if (WBCTversion == 2)
  { // tint, offset, as shot R, as shot B, CСT
    if ((unique_id == 0x03740000ULL) || // M3
        (unique_id == 0x03840000ULL) || // M10
        (imCanon.ColorDataSubVer == 0xfffc))
    {
      for (i = 0; i < 15; i++)
      {
        fseek(ifp, 4, SEEK_CUR);
        imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] =
            1.0f;
        imgdata.color.WBCT_Coeffs[i][1] = 1024.0f / fMAX(1.f, get2());
        imgdata.color.WBCT_Coeffs[i][3] = 1024.0f / fMAX(1.f, get2());
        imgdata.color.WBCT_Coeffs[i][0] = get2();
      }
    }
    else if (imCanon.ColorDataSubVer == 0xfffd)
    {
      for (i = 0; i < 15; i++)
      {
        fseek(ifp, 2, SEEK_CUR);
        norm = (signed short)get2();
        norm = 512.0f + norm / 8.0f;
        imgdata.color.WBCT_Coeffs[i][2] = imgdata.color.WBCT_Coeffs[i][4] =
            1.0f;
        imgdata.color.WBCT_Coeffs[i][1] = (float)get2();
        if (norm > 0.001f)
          imgdata.color.WBCT_Coeffs[i][1] /= norm;
        imgdata.color.WBCT_Coeffs[i][3] = (float)get2();
        if (norm > 0.001f)
          imgdata.color.WBCT_Coeffs[i][3] /= norm;
        imgdata.color.WBCT_Coeffs[i][0] = get2();
      }
    }
  }
  return;
}