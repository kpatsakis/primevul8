void LibRaw::parseSonyLensFeatures(uchar a, uchar b)
{

  ushort features;
  features = (((ushort)a) << 8) | ((ushort)b);

  if ((ilm.LensMount == LIBRAW_MOUNT_Canon_EF) ||
      (ilm.LensMount != LIBRAW_MOUNT_Sigma_X3F) || !features)
    return;

  ilm.LensFeatures_pre[0] = 0;
  ilm.LensFeatures_suf[0] = 0;
  if ((features & 0x0200) && (features & 0x0100))
    strcpy(ilm.LensFeatures_pre, "E");
  else if (features & 0x0200)
    strcpy(ilm.LensFeatures_pre, "FE");
  else if (features & 0x0100)
    strcpy(ilm.LensFeatures_pre, "DT");

  if (!ilm.LensFormat && !ilm.LensMount)
  {
    ilm.LensFormat = LIBRAW_FORMAT_FF;
    ilm.LensMount = LIBRAW_MOUNT_Minolta_A;

    if ((features & 0x0200) && (features & 0x0100))
    {
      ilm.LensFormat = LIBRAW_FORMAT_APSC;
      ilm.LensMount = LIBRAW_MOUNT_Sony_E;
    }
    else if (features & 0x0200)
    {
      ilm.LensMount = LIBRAW_MOUNT_Sony_E;
    }
    else if (features & 0x0100)
    {
      ilm.LensFormat = LIBRAW_FORMAT_APSC;
    }
  }

  if (features & 0x4000)
    strnXcat(ilm.LensFeatures_pre, " PZ");

  if (features & 0x0008)
    strnXcat(ilm.LensFeatures_suf, " G");
  else if (features & 0x0004)
    strnXcat(ilm.LensFeatures_suf, " ZA");

  if ((features & 0x0020) && (features & 0x0040))
    strnXcat(ilm.LensFeatures_suf, " Macro");
  else if (features & 0x0020)
    strnXcat(ilm.LensFeatures_suf, " STF");
  else if (features & 0x0040)
    strnXcat(ilm.LensFeatures_suf, " Reflex");
  else if (features & 0x0080)
    strnXcat(ilm.LensFeatures_suf, " Fisheye");

  if (features & 0x0001)
    strnXcat(ilm.LensFeatures_suf, " SSM");
  else if (features & 0x0002)
    strnXcat(ilm.LensFeatures_suf, " SAM");

  if (features & 0x8000)
    strnXcat(ilm.LensFeatures_suf, " OSS");

  if (features & 0x2000)
    strnXcat(ilm.LensFeatures_suf, " LE");

  if (features & 0x0800)
    strnXcat(ilm.LensFeatures_suf, " II");

  if (ilm.LensFeatures_suf[0] == ' ')
    memmove(ilm.LensFeatures_suf, ilm.LensFeatures_suf + 1,
            strbuflen(ilm.LensFeatures_suf) - 1);

  return;
}