void LibRaw::processNikonLensData(uchar *LensData, unsigned len)
{

  ushort i;

  if (!(imgdata.lens.nikon.LensType & 0x01))
  {
    ilm.LensFeatures_pre[0] = 'A';
    ilm.LensFeatures_pre[1] = 'F';
  }
  else
  {
    ilm.LensFeatures_pre[0] = 'M';
    ilm.LensFeatures_pre[1] = 'F';
  }

  if (imgdata.lens.nikon.LensType & 0x02)
  {
    if (imgdata.lens.nikon.LensType & 0x04)
      ilm.LensFeatures_suf[0] = 'G';
    else
      ilm.LensFeatures_suf[0] = 'D';
    ilm.LensFeatures_suf[1] = ' ';
  }

  if (imgdata.lens.nikon.LensType & 0x08)
  {
    ilm.LensFeatures_suf[2] = 'V';
    ilm.LensFeatures_suf[3] = 'R';
  }

  if (imgdata.lens.nikon.LensType & 0x10)
  {
    ilm.LensMount = ilm.CameraMount = LIBRAW_MOUNT_Nikon_CX;
    ilm.CameraFormat = ilm.LensFormat = LIBRAW_FORMAT_1INCH;
  }
  else
    ilm.LensMount = ilm.CameraMount = LIBRAW_MOUNT_Nikon_F;

  if (imgdata.lens.nikon.LensType & 0x20)
  {
    strcpy(ilm.Adapter, "FT-1");
    ilm.LensMount = LIBRAW_MOUNT_Nikon_F;
    ilm.CameraMount = LIBRAW_MOUNT_Nikon_CX;
    ilm.CameraFormat = LIBRAW_FORMAT_1INCH;
  }

  imgdata.lens.nikon.LensType = imgdata.lens.nikon.LensType & 0xdf;

  if ((len < 20) || (len == 58))
  {
    switch (len)
    {
    case 9:
      i = 2;
      break;
    case 15:
      i = 7;
      break;
    case 16:
      i = 8;
      break;
    case 58: // "Z 6", "Z 7"
      ilm.CameraMount = LIBRAW_MOUNT_Nikon_Z;
      ilm.CameraFormat = LIBRAW_FORMAT_FF;
      i = 1;
      while ((LensData[i] == LensData[0]) && (i < 17))
        i++;
      if (i == 17)
      {
        ilm.LensMount = LIBRAW_MOUNT_Nikon_Z;
        ilm.LensFormat = LIBRAW_FORMAT_FF;
        ilm.LensID = sget2(LensData + 0x2c);
        if (ilm.MaxAp4CurFocal < 0.7f)
          ilm.MaxAp4CurFocal = libraw_powf64l(
              2.0f, (float)sget2(LensData + 0x32) / 384.0f - 1.0f);
        if (ilm.CurAp < 0.7f)
          ilm.CurAp = libraw_powf64l(
              2.0f, (float)sget2(LensData + 0x34) / 384.0f - 1.0f);
        if (fabsf(ilm.CurFocal) < 1.1f)
          ilm.CurFocal = sget2(LensData + 0x38);
        return;
      }
      i = 9;
      ilm.LensMount = LIBRAW_MOUNT_Nikon_F;
      strcpy(ilm.Adapter, "FTZ");
      break;
    }
    imgdata.lens.nikon.LensIDNumber = LensData[i];
    imgdata.lens.nikon.LensFStops = LensData[i + 1];
    ilm.LensFStops = (float)imgdata.lens.nikon.LensFStops / 12.0f;
    if (fabsf(ilm.MinFocal) < 1.1f)
    {
      if ((imgdata.lens.nikon.LensType ^ (uchar)0x01) || LensData[i + 2])
        ilm.MinFocal =
            5.0f * libraw_powf64l(2.0f, (float)LensData[i + 2] / 24.0f);
      if ((imgdata.lens.nikon.LensType ^ (uchar)0x01) || LensData[i + 3])
        ilm.MaxFocal =
            5.0f * libraw_powf64l(2.0f, (float)LensData[i + 3] / 24.0f);
      if ((imgdata.lens.nikon.LensType ^ (uchar)0x01) || LensData[i + 4])
        ilm.MaxAp4MinFocal =
            libraw_powf64l(2.0f, (float)LensData[i + 4] / 24.0f);
      if ((imgdata.lens.nikon.LensType ^ (uchar)0x01) || LensData[i + 5])
        ilm.MaxAp4MaxFocal =
            libraw_powf64l(2.0f, (float)LensData[i + 5] / 24.0f);
    }
    imgdata.lens.nikon.MCUVersion = LensData[i + 6];
    if (i != 2)
    {
      if ((LensData[i - 1]) && (fabsf(ilm.CurFocal) < 1.1f))
        ilm.CurFocal =
            5.0f * libraw_powf64l(2.0f, (float)LensData[i - 1] / 24.0f);
      if (LensData[i + 7])
        imgdata.lens.nikon.EffectiveMaxAp =
            libraw_powf64l(2.0f, (float)LensData[i + 7] / 24.0f);
    }
    ilm.LensID = (unsigned long long)LensData[i] << 56 |
                 (unsigned long long)LensData[i + 1] << 48 |
                 (unsigned long long)LensData[i + 2] << 40 |
                 (unsigned long long)LensData[i + 3] << 32 |
                 (unsigned long long)LensData[i + 4] << 24 |
                 (unsigned long long)LensData[i + 5] << 16 |
                 (unsigned long long)LensData[i + 6] << 8 |
                 (unsigned long long)imgdata.lens.nikon.LensType;
  }
  else if ((len == 459) || (len == 590))
  {
    memcpy(ilm.Lens, LensData + 390, 64);
  }
  else if (len == 509)
  {
    memcpy(ilm.Lens, LensData + 391, 64);
  }
  else if (len == 879)
  {
    memcpy(ilm.Lens, LensData + 680, 64);
  }

  return;
}