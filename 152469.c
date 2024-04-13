void CLASS parseSonyLensFeatures (uchar a, uchar b) {

  ushort features;
  features = (((ushort)a)<<8) | ((ushort)b);

  if ((imgdata.lens.makernotes.LensMount == LIBRAW_MOUNT_Canon_EF) || !features)
    return;

  imgdata.lens.makernotes.LensFeatures_pre[0] = 0;
  imgdata.lens.makernotes.LensFeatures_suf[0] = 0;
  if ((features & 0x0200) && (features & 0x0100)) strcpy(imgdata.lens.makernotes.LensFeatures_pre, "E");
  else if (features & 0x0200) strcpy(imgdata.lens.makernotes.LensFeatures_pre, "FE");
  else if (features & 0x0100) strcpy(imgdata.lens.makernotes.LensFeatures_pre, "DT");

  if (!imgdata.lens.makernotes.LensFormat && !imgdata.lens.makernotes.LensMount)
    {
  	  imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_FF;
  	  imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Minolta_A;

  	  if ((features & 0x0200) && (features & 0x0100)) {
  		imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
  		imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E;
  	  } else if (features & 0x0200) {
  		imgdata.lens.makernotes.LensMount = LIBRAW_MOUNT_Sony_E;
  	  } else if (features & 0x0100) {
  		imgdata.lens.makernotes.LensFormat = LIBRAW_FORMAT_APSC;
  	  }
    }

  if (features & 0x4000)
    strncat(imgdata.lens.makernotes.LensFeatures_pre, " PZ", sizeof(imgdata.lens.makernotes.LensFeatures_pre));

  if (features & 0x0008)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " G", sizeof(imgdata.lens.makernotes.LensFeatures_suf));
  else if (features & 0x0004)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " ZA", sizeof(imgdata.lens.makernotes.LensFeatures_suf));

  if ((features & 0x0020) && (features & 0x0040))
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " Macro", sizeof(imgdata.lens.makernotes.LensFeatures_suf));
  else if (features & 0x0020)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " STF", sizeof(imgdata.lens.makernotes.LensFeatures_suf));
  else if (features & 0x0040)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " Reflex", sizeof(imgdata.lens.makernotes.LensFeatures_suf));
  else if (features & 0x0080)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " Fisheye", sizeof(imgdata.lens.makernotes.LensFeatures_suf));

  if (features & 0x0001)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " SSM", sizeof(imgdata.lens.makernotes.LensFeatures_suf));
  else if (features & 0x0002)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " SAM", sizeof(imgdata.lens.makernotes.LensFeatures_suf));

  if (features & 0x8000)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " OSS", sizeof(imgdata.lens.makernotes.LensFeatures_suf));

  if (features & 0x2000)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " LE", sizeof(imgdata.lens.makernotes.LensFeatures_suf));

  if (features & 0x0800)
    strncat(imgdata.lens.makernotes.LensFeatures_suf, " II", sizeof(imgdata.lens.makernotes.LensFeatures_suf));

  if (imgdata.lens.makernotes.LensFeatures_suf[0] == ' ')
    memmove(imgdata.lens.makernotes.LensFeatures_suf, imgdata.lens.makernotes.LensFeatures_suf+1, strlen(imgdata.lens.makernotes.LensFeatures_suf));

  return;
}