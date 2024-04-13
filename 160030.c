void LibRaw::process_Sony_0x9050(uchar *buf, ushort len, unsigned long long id)
{
  ushort lid;
  uchar s[4];
  int c;

  if ((ilm.CameraMount != LIBRAW_MOUNT_Sony_E) &&
      (ilm.CameraMount != LIBRAW_MOUNT_FixedLens))
  {
    if (len < 2)
      return;
    if (buf[0])
      ilm.MaxAp4CurFocal =
          my_roundf(libraw_powf64l(
                        2.0f, ((float)SonySubstitution[buf[0]] / 8.0 - 1.06f) /
                                  2.0f) *
                    10.0f) /
          10.0f;

    if (buf[1])
      ilm.MinAp4CurFocal =
          my_roundf(libraw_powf64l(
                        2.0f, ((float)SonySubstitution[buf[1]] / 8.0 - 1.06f) /
                                  2.0f) *
                    10.0f) /
          10.0f;
  }

  if (ilm.CameraMount != LIBRAW_MOUNT_FixedLens)
  {
    if (len <= 0x106)
      return;
    if (buf[0x3d] | buf[0x3c])
    {
      lid = SonySubstitution[buf[0x3d]] << 8 | SonySubstitution[buf[0x3c]];
      ilm.CurAp = libraw_powf64l(2.0f, ((float)lid / 256.0f - 16.0f) / 2.0f);
    }
    if (buf[0x105] && (ilm.LensMount != LIBRAW_MOUNT_Canon_EF) &&
        (ilm.LensMount != LIBRAW_MOUNT_Sigma_X3F))
      ilm.LensMount = SonySubstitution[buf[0x105]];
    if (buf[0x106])
      ilm.LensFormat = SonySubstitution[buf[0x106]];
  }

  if (ilm.CameraMount == LIBRAW_MOUNT_Sony_E)
  {
    if (len <= 0x108)
      return;
    parseSonyLensType2(
        SonySubstitution[buf[0x0108]], // LensType2 - Sony lens ids
        SonySubstitution[buf[0x0107]]);
  }

  if (len <= 0x10a)
    return;
  if ((ilm.LensID == -1) && (ilm.CameraMount == LIBRAW_MOUNT_Minolta_A) &&
      (buf[0x010a] | buf[0x0109]))
  {
    ilm.LensID = // LensType - Minolta/Sony lens ids
        SonySubstitution[buf[0x010a]] << 8 | SonySubstitution[buf[0x0109]];

    if ((ilm.LensID > 0x4900) && (ilm.LensID <= 0x5900))
    {
      ilm.AdapterID = 0x4900;
      ilm.LensID -= ilm.AdapterID;
      ilm.LensMount = LIBRAW_MOUNT_Sigma_X3F;
      strcpy(ilm.Adapter, "MC-11");
    }

    else if ((ilm.LensID > 0xef00) && (ilm.LensID < 0xffff) &&
             (ilm.LensID != 0xff00))
    {
      ilm.AdapterID = 0xef00;
      ilm.LensID -= ilm.AdapterID;
      ilm.LensMount = LIBRAW_MOUNT_Canon_EF;
    }
  }

  if ((id >= 0x11eULL) && (id <= 0x125ULL))
  {
    if (len <= 0x116)
      return;
    // "SLT-A65", "SLT-A77", "NEX-7", "NEX-VG20",
    // "SLT-A37", "SLT-A57", "NEX-F3", "Lunar"
    parseSonyLensFeatures(SonySubstitution[buf[0x115]],
                          SonySubstitution[buf[0x116]]);
  }
  else if (ilm.CameraMount != LIBRAW_MOUNT_FixedLens)
  {
    if (len <= 0x117)
      return;
    parseSonyLensFeatures(SonySubstitution[buf[0x116]],
                          SonySubstitution[buf[0x117]]);
  }

  if ((id == 0x15bULL) || // ILCE-7RM2
      (id == 0x15eULL) || // ILCE-7SM2
      (id == 0x162ULL) || // ILCA-99M2
      (id == 0x165ULL) || // ILCE-6300
      (id == 0x166ULL) || // ILCE-9
      (id == 0x168ULL) || // ILCE-6500
      (id == 0x16aULL) || // ILCE-7RM3
      (id == 0x16bULL) || // ILCE-7M3
      (id == 0x173ULL) || // ILCE-6400
      (id == 0x177ULL) || // ILCE-7RM4
      (id == 0x17aULL) || // ILCE-6600
      (id == 0x17bULL)    // ILCE-6100
  )
  {
    if (len <= 0x8d)
      return;
    unsigned long long b88 = SonySubstitution[buf[0x88]];
    unsigned long long b89 = SonySubstitution[buf[0x89]];
    unsigned long long b8a = SonySubstitution[buf[0x8a]];
    unsigned long long b8b = SonySubstitution[buf[0x8b]];
    unsigned long long b8c = SonySubstitution[buf[0x8c]];
    unsigned long long b8d = SonySubstitution[buf[0x8d]];
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%06llx",
            (b88 << 40) + (b89 << 32) + (b8a << 24) + (b8b << 16) + (b8c << 8) +
                b8d);
  }
  else if (ilm.CameraMount == LIBRAW_MOUNT_Minolta_A)
  {
    if (len <= 0xf4)
      return;
    unsigned long long bf0 = SonySubstitution[buf[0xf0]];
    unsigned long long bf1 = SonySubstitution[buf[0xf1]];
    unsigned long long bf2 = SonySubstitution[buf[0xf2]];
    unsigned long long bf3 = SonySubstitution[buf[0xf3]];
    unsigned long long bf4 = SonySubstitution[buf[0xf4]];
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%05llx",
            (bf0 << 32) + (bf1 << 24) + (bf2 << 16) + (bf3 << 8) + bf4);
  }
  else if ((ilm.CameraMount == LIBRAW_MOUNT_Sony_E) &&
           (id != 0x120ULL) && // not NEX-5N
           (id != 0x121ULL) && // not NEX-7
           (id != 0x122ULL)    // not NEX-VG20
  )
  {
    if (len <= 0x7f)
      return;
    unsigned b7c = SonySubstitution[buf[0x7c]];
    unsigned b7d = SonySubstitution[buf[0x7d]];
    unsigned b7e = SonySubstitution[buf[0x7e]];
    unsigned b7f = SonySubstitution[buf[0x7f]];
    sprintf(imgdata.shootinginfo.InternalBodySerial, "%04x",
            (b7c << 24) + (b7d << 16) + (b7e << 8) + b7f);
  }

  if ((imSony.ImageCount3_offset != 0xffff) &&
      (len >= (imSony.ImageCount3_offset + 4)))
  {
    FORC4 s[c] = SonySubstitution[buf[imSony.ImageCount3_offset + c]];
    imSony.ImageCount3 = sget4(s);
  }

  return;
}