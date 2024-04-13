void CLASS parsePentaxMakernotes(int base, unsigned tag, unsigned type, unsigned len, unsigned dng_writer)
{
  int c;
  if (tag == 0x0005) {
      unique_id = get4();
      setPentaxBodyFeatures(unique_id);
  } else if (tag == 0x0008) {  /* 4 is raw, 7 is raw w/ pixel shift, 8 is raw w/ dynamic pixel shift */
    imgdata.makernotes.pentax.Quality = get2();
  } else if (tag == 0x000d) {
      imgdata.shootinginfo.FocusMode = imgdata.makernotes.pentax.FocusMode = get2();
  } else if (tag == 0x000e) {
      imgdata.shootinginfo.AFPoint = imgdata.makernotes.pentax.AFPointSelected = get2();
  } else if (tag == 0x000f) {
      imgdata.makernotes.pentax.AFPointsInFocus = getint(type);
  } else if (tag == 0x0010) {
      imgdata.makernotes.pentax.FocusPosition = get2();
  } else if (tag == 0x0013) {
      imgdata.lens.makernotes.CurAp = (float)get2() / 10.0f;
  } else if (tag == 0x0014) {
      PentaxISO(get2());
  } else if (tag == 0x0017) {
      imgdata.shootinginfo.MeteringMode = get2();
  } else if (tag == 0x001d) {
      imgdata.lens.makernotes.CurFocal = (float)get4() / 100.0f;
  } else if (tag == 0x0034) {
      uchar uc;
      FORC4 {
        fread(&uc, 1, 1, ifp);
        imgdata.makernotes.pentax.DriveMode[c] = uc;
      }
      imgdata.shootinginfo.DriveMode = imgdata.makernotes.pentax.DriveMode[0];
  } else if (tag == 0x0038) {
      imgdata.sizes.raw_crop.cleft = get2();
      imgdata.sizes.raw_crop.ctop = get2();
  } else if (tag == 0x0039) {
      imgdata.sizes.raw_crop.cwidth = get2();
      imgdata.sizes.raw_crop.cheight = get2();
  } else if (tag == 0x003f) {
      imgdata.lens.makernotes.LensID = fgetc(ifp) << 8 | fgetc(ifp);
  } else if (tag == 0x0047) {
      imgdata.other.CameraTemperature = (float)fgetc(ifp);
  } else if (tag == 0x004d) {
      if (type == 9)
        imgdata.other.FlashEC = getreal(type) / 256.0f;
      else
        imgdata.other.FlashEC = (float)((signed short)fgetc(ifp)) / 6.0f;
  } else if (tag == 0x005c) {
      fgetc(ifp);
      imgdata.shootinginfo.ImageStabilization = (short)fgetc(ifp);
  } else if (tag == 0x0072) {
      imgdata.makernotes.pentax.AFAdjustment = get2();
  } else if (tag == 0x007e) {
      imgdata.color.linear_max[0] =
        imgdata.color.linear_max[1] =
        imgdata.color.linear_max[2] =
        imgdata.color.linear_max[3] = (long)(-1) * get4();
  } else if ((tag == 0x0203) && (dng_writer == nonDNG)) {
      for (int i = 0; i < 3; i++)
        FORC3 cmatrix[i][c] = ((short)get2()) / 8192.0;
  } else if (tag == 0x0205) {
      if (len < 25) {
        fseek (ifp, 10, SEEK_CUR);
        imgdata.makernotes.pentax.MultiExposure = (fgetc(ifp) & 0x0f);
      }
  } else if (tag == 0x0207) {
      if (len < 65535) // Safety belt
        PentaxLensInfo(imgdata.lens.makernotes.CamID, len);
  } else if ((tag >= 0x020d) && (tag <= 0x0214)) {
      FORC4 imgdata.color.WB_Coeffs[Pentax_wb_list1[tag - 0x020d]][c ^ (c >> 1)] = get2();
  } else if (tag == 0x0221) {
      int nWB = get2();
      if (nWB <= sizeof(imgdata.color.WBCT_Coeffs) / sizeof(imgdata.color.WBCT_Coeffs[0]))
      FORC(nWB) {
        imgdata.color.WBCT_Coeffs[c][0] = (unsigned)0xcfc6 - get2();
        fseek(ifp, 2, SEEK_CUR);
        imgdata.color.WBCT_Coeffs[c][1] = get2();
        imgdata.color.WBCT_Coeffs[c][2] = imgdata.color.WBCT_Coeffs[c][4] = 0x2000;
        imgdata.color.WBCT_Coeffs[c][3] = get2();
      }
  } else if (tag == 0x0215) {
      fseek(ifp, 16, SEEK_CUR);
      sprintf(imgdata.shootinginfo.InternalBodySerial, "%d", get4());
  } else if (tag == 0x0229) {
      stmread(imgdata.shootinginfo.BodySerial, len, ifp);
  } else if (tag == 0x022d) {
      int wb_ind;
      getc(ifp);
      for (int wb_cnt = 0; wb_cnt < nPentax_wb_list2; wb_cnt++) {
        wb_ind = getc(ifp);
        if (wb_ind < nPentax_wb_list2)
          FORC4 imgdata.color.WB_Coeffs[Pentax_wb_list2[wb_ind]][c ^ (c >> 1)] = get2();
      }
  } else if (tag == 0x0239) {  // Q-series lens info (LensInfoQ)
      char LensInfo[20];
      fseek(ifp, 12, SEEK_CUR);
      stread(imgdata.lens.makernotes.Lens, 30, ifp);
      strcat(imgdata.lens.makernotes.Lens, " ");
      stread(LensInfo, 20, ifp);
      strcat(imgdata.lens.makernotes.Lens, LensInfo);
  }
}