void LibRaw::parsePentaxMakernotes(int base, unsigned tag, unsigned type,
                                   unsigned len, unsigned dng_writer)
{

  int c;
  if (tag == 0x0005)
  {
    unique_id = get4();
    setPentaxBodyFeatures(unique_id);
  }
  else if (tag == 0x0008)
  { /* 4 is raw, 7 is raw w/ pixel shift, 8 is raw w/ dynamic pixel shift */
    imPentax.Quality = get2();
  }
  else if (tag == 0x000d)
  {
    imgdata.shootinginfo.FocusMode = imPentax.FocusMode = get2();
  }
  else if (tag == 0x000e)
  {
    imgdata.shootinginfo.AFPoint = imPentax.AFPointSelected = get2();
  }
  else if (tag == 0x000f)
  {
    imPentax.AFPointsInFocus = getint(type);
  }
  else if (tag == 0x0010)
  {
    imPentax.FocusPosition = get2();
  }
  else if (tag == 0x0013)
  {
    ilm.CurAp = (float)get2() / 10.0f;
  }
  else if (tag == 0x0014)
  {
    PentaxISO(get2());
  }
  else if (tag == 0x0017)
  {
    imgdata.shootinginfo.MeteringMode = get2();
  }
  else if (tag == 0x001b) {
    cam_mul[2] = get2() / 256.0;
  }
  else if (tag == 0x001c) {
    cam_mul[0] = get2() / 256.0;
  }
  else if (tag == 0x001d)
  {
    ilm.CurFocal = (float)get4() / 100.0f;
  }
  else if (tag == 0x0034)
  {
    uchar uc;
    FORC4
    {
      fread(&uc, 1, 1, ifp);
      imPentax.DriveMode[c] = uc;
    }
    imgdata.shootinginfo.DriveMode = imPentax.DriveMode[0];
  }
  else if (tag == 0x0038)
  {
    imgdata.sizes.raw_inset_crop.cleft = get2();
    imgdata.sizes.raw_inset_crop.ctop = get2();
  }
  else if (tag == 0x0039)
  {
    imgdata.sizes.raw_inset_crop.cwidth = get2();
    imgdata.sizes.raw_inset_crop.cheight = get2();
  }
  else if (tag == 0x003f)
  {
    unsigned a = fgetc(ifp) << 8;
    ilm.LensID = a | fgetc(ifp);
  }
  else if (tag == 0x0047)
  {
    imgdata.makernotes.common.CameraTemperature = (float)fgetc(ifp);
  }
  else if (tag == 0x004d)
  {
    if (type == 9)
      imgdata.makernotes.common.FlashEC = getreal(type) / 256.0f;
    else
      imgdata.makernotes.common.FlashEC = (float)((signed short)fgetc(ifp)) / 6.0f;
  }
  else if (tag == 0x005c)
  {
    fgetc(ifp);
    imgdata.shootinginfo.ImageStabilization = (short)fgetc(ifp);
  }
  else if (tag == 0x0072)
  {
    imPentax.AFAdjustment = get2();
  }
  else if ((tag == 0x007e) && (dng_writer == nonDNG))
  {
    imgdata.color.linear_max[0] = imgdata.color.linear_max[1] =
        imgdata.color.linear_max[2] = imgdata.color.linear_max[3] =
            get4();
  }
  else if (tag == 0x0080)
  {
    short a = (short)fgetc(ifp);
    switch (a)
    {
    case 0:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_4to3;
      break;
    case 1:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_3to2;
      break;
    case 2:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_16to9;
      break;
    case 3:
      imgdata.sizes.raw_inset_crop.aspect = LIBRAW_IMAGE_ASPECT_1to1;
      break;
    }
  }

  else if ((tag == 0x0200) && (dng_writer == nonDNG)) { // Pentax black level
    FORC4 cblack[c ^ c >> 1] = get2();
  }

  else if ((tag == 0x0201) && (dng_writer == nonDNG)) { // Pentax As Shot WB
    FORC4 cam_mul[c ^ (c >> 1)] = get2();
  }

  else if ((tag == 0x0203) && (dng_writer == nonDNG))
  {
    for (int i = 0; i < 3; i++)
      FORC3 cmatrix[i][c] = ((short)get2()) / 8192.0;
  }
  else if (tag == 0x0205)
  { // Pentax K-m has multiexposure set to 8 when no multi-exposure is in effect
    if (len < 25)
    {
      fseek(ifp, 10, SEEK_CUR);
      imPentax.MultiExposure = fgetc(ifp) & 0x0f;
    }
  }
  else if (tag == 0x0207)
  {
    if (len < 65535) // Safety belt
      PentaxLensInfo(ilm.CamID, len);
  }
  else if ((tag >= 0x020d) && (tag <= 0x0214))
  {
    FORC4 imgdata.color.WB_Coeffs[Pentax_wb_list1[tag - 0x020d]][c ^ (c >> 1)] =
        get2();
  }

  else if ((tag == 0x0220) && (dng_writer == nonDNG)) {
    meta_offset = ftell(ifp);
  }

  else if (tag == 0x0221)
  {
    int nWB = get2();
    if (nWB <= sizeof(imgdata.color.WBCT_Coeffs) /
                   sizeof(imgdata.color.WBCT_Coeffs[0]))
      FORC(nWB)
      {
        imgdata.color.WBCT_Coeffs[c][0] = (unsigned)0xcfc6 - get2();
        fseek(ifp, 2, SEEK_CUR);
        imgdata.color.WBCT_Coeffs[c][1] = get2();
        imgdata.color.WBCT_Coeffs[c][2] = imgdata.color.WBCT_Coeffs[c][4] =
            0x2000;
        imgdata.color.WBCT_Coeffs[c][3] = get2();
      }
  }
  else if (tag == 0x0215)
  {
    fseek(ifp, 16, SEEK_CUR);
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%d", get4());
  }
  else if (tag == 0x0229)
  {
    stmread(imgdata.shootinginfo.BodySerial, len, ifp);
  }
  else if (tag == 0x022d)
  {
    int wb_ind;
    getc(ifp);
    for (int wb_cnt = 0; wb_cnt < nPentax_wb_list2; wb_cnt++)
    {
      wb_ind = getc(ifp);
      if (wb_ind >= 0 && wb_ind < nPentax_wb_list2)
        FORC4 imgdata.color.WB_Coeffs[Pentax_wb_list2[wb_ind]][c ^ (c >> 1)] =
            get2();
    }
  }
  else if (tag == 0x0239)
  { // Q-series lens info (LensInfoQ)
    char LensInfo[20];
    fseek(ifp, 12, SEEK_CUR);
    stread(ilm.Lens, 30, ifp);
    strcat(ilm.Lens, " ");
    stread(LensInfo, 20, ifp);
    strcat(ilm.Lens, LensInfo);
  }
}