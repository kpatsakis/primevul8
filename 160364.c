void LibRaw::setLeicaBodyFeatures(int LeicaMakernoteSignature)
{
  if (LeicaMakernoteSignature == -3)
  { // M8
    ilm.CameraFormat = LIBRAW_FORMAT_APSH;
    ilm.CameraMount = LIBRAW_MOUNT_Leica_M;
  }
  else if (LeicaMakernoteSignature == -2)
  { // DMR
    ilm.CameraFormat = LIBRAW_FORMAT_Leica_DMR;
    if ((model[0] == 'R') || (model[6] == 'R'))
      ilm.CameraMount = LIBRAW_MOUNT_Leica_R;
  }
  else if (LeicaMakernoteSignature == 0)
  { // DIGILUX 2
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.FocalType = LIBRAW_FT_ZOOM_LENS;
  }
  else if ((LeicaMakernoteSignature == 0x0100) || // X1
           (LeicaMakernoteSignature == 0x0500) || // X2, X-E (Typ 102)
           (LeicaMakernoteSignature == 0x0700) || // X (Typ 113)
           (LeicaMakernoteSignature == 0x1000))
  { // X-U (Typ 113)
    ilm.CameraFormat = ilm.LensFormat = LIBRAW_FORMAT_APSC;
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.FocalType = LIBRAW_FT_PRIME_LENS;
  }
  else if (LeicaMakernoteSignature == 0x0400)
  { // X VARIO (Typ 107)
    ilm.CameraFormat = ilm.LensFormat = LIBRAW_FORMAT_APSC;
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.FocalType = LIBRAW_FT_ZOOM_LENS;
  }
  else if ((LeicaMakernoteSignature == 0x0200) || // M10, M10-D, S (Typ 007)
           (LeicaMakernoteSignature ==
            0x02ff) || // M (Typ 240), M (Typ 262), M-D (Typ 262), M Monochrom
                       // (Typ 246), S (Typ 006), S-E (Typ 006), S2, S3
           (LeicaMakernoteSignature == 0x0300))
  { // M9, M9 Monochrom, M Monochrom, M-E
    if ((model[0] == 'M') || (model[6] == 'M'))
    {
      ilm.CameraFormat = LIBRAW_FORMAT_FF;
      ilm.CameraMount = LIBRAW_MOUNT_Leica_M;
    }
    else if ((model[0] == 'S') || (model[6] == 'S'))
    {
      ilm.CameraFormat = LIBRAW_FORMAT_LeicaS;
      ilm.CameraMount = LIBRAW_MOUNT_Leica_S;
    }
  }
  else if ((LeicaMakernoteSignature == 0x0600) || // T (Typ 701), TL
           (LeicaMakernoteSignature == 0x0900) || // SL (Typ 601), CL, Q2
           (LeicaMakernoteSignature == 0x1a00))
  { // TL2
    if ((model[0] == 'S') || (model[6] == 'S'))
    {
      ilm.CameraFormat = LIBRAW_FORMAT_FF;
      ilm.CameraMount = LIBRAW_MOUNT_Leica_L;
    }
    else if ((model[0] == 'T') || (model[6] == 'T') || (model[0] == 'C') ||
             (model[6] == 'C'))
    {
      ilm.CameraFormat = LIBRAW_FORMAT_APSC;
      ilm.CameraMount = LIBRAW_MOUNT_Leica_L;
    }
    else if (((model[0] == 'Q') || (model[6] == 'Q')) &&
             ((model[1] == '2') || (model[7] == '2')))
    {
      ilm.CameraFormat = ilm.LensFormat = LIBRAW_FORMAT_FF;
      ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
      ilm.FocalType = LIBRAW_FT_PRIME_LENS;
    }
  }
  else if (LeicaMakernoteSignature == 0x0800)
  { // Q (Typ 116)
    ilm.CameraFormat = ilm.LensFormat = LIBRAW_FORMAT_FF;
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    ilm.FocalType = LIBRAW_FT_PRIME_LENS;
  }
}