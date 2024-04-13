void CLASS process_Sony_0x9050 (uchar * buf, unsigned id)
{
  ushort lid;

  if ((imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_Sony_E) &&
      (imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_FixedLens))
    {
      if (buf[0])
        imgdata.lens.makernotes.MaxAp4CurFocal =
          my_roundf(powf64(2.0f, ((float)SonySubstitution[buf[0]] / 8.0 - 1.06f) / 2.0f)*10.0f) / 10.0f;

      if (buf[1])
        imgdata.lens.makernotes.MinAp4CurFocal =
          my_roundf(powf64(2.0f, ((float)SonySubstitution[buf[1]] / 8.0 - 1.06f) / 2.0f)*10.0f) / 10.0f;
    }

  if (imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_FixedLens)
    {
      if (buf[0x3d] | buf[0x3c])
        {
          lid = SonySubstitution[buf[0x3d]] << 8 |
            SonySubstitution[buf[0x3c]];
          imgdata.lens.makernotes.CurAp =
            powf64(2.0f, ((float)lid/256.0f - 16.0f) / 2.0f);
        }
      if (buf[0x105] && (imgdata.lens.makernotes.LensMount != LIBRAW_MOUNT_Canon_EF))
        imgdata.lens.makernotes.LensMount =
          SonySubstitution[buf[0x105]];
      if (buf[0x106])
        imgdata.lens.makernotes.LensFormat =
          SonySubstitution[buf[0x106]];
    }

  if (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Sony_E)
    {
      parseSonyLensType2 (SonySubstitution[buf[0x0108]],		// LensType2 - Sony lens ids
                          SonySubstitution[buf[0x0107]]);
    }

  if ((imgdata.lens.makernotes.LensID == -1) &&
      (imgdata.lens.makernotes.CameraMount == LIBRAW_MOUNT_Minolta_A) &&
      (buf[0x010a] | buf[0x0109]))
    {
      imgdata.lens.makernotes.LensID =		 // LensType - Minolta/Sony lens ids
        SonySubstitution[buf[0x010a]] << 8 |
        SonySubstitution[buf[0x0109]];
      if ((imgdata.lens.makernotes.LensID > 61184) &&
          (imgdata.lens.makernotes.LensID < 65535))
        {
          imgdata.lens.makernotes.LensID -= 61184;
          imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Canon_EF;
        }
    }

  if ((id >= 286) && (id <= 293))
    // "SLT-A65", "SLT-A77", "NEX-7", "NEX-VG20E",
    // "SLT-A37", "SLT-A57", "NEX-F3", "Lunar"
    parseSonyLensFeatures (SonySubstitution[buf[0x115]],
                           SonySubstitution[buf[0x116]]);
  else if (imgdata.lens.makernotes.CameraMount != LIBRAW_MOUNT_FixedLens)
    parseSonyLensFeatures (SonySubstitution[buf[0x116]],
                           SonySubstitution[buf[0x117]]);
  return;
}