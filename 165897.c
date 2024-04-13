void CLASS setSonyBodyFeatures(unsigned id)
{
  ushort idx;
  static const struct
  {
    ushort scf[11];
    /*
    scf[0]  camera id
    scf[1]  camera format
    scf[2]  camera mount: Minolta A, Sony E, fixed,
    scf[3]  camera type: DSLR, NEX, SLT, ILCE, ILCA, DSC
    scf[4]  lens mount
    scf[5]  tag 0x2010 group (0 if not used)
    scf[6]  offset of Sony ISO in 0x2010 table, 0xffff if not valid
    scf[7]  offset of ImageCount3 in 0x9050 table, 0xffff if not valid
    scf[8]  offset of MeteringMode in 0x2010 table, 0xffff if not valid
    scf[9]  offset of ExposureProgram in 0x2010 table, 0xffff if not valid
    scf[10] offset of ReleaseMode2 in 0x2010 table, 0xffff if not valid
    */
  } SonyCamFeatures[] = {
      {256, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {257, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {258, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {259, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {260, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {261, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {262, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {263, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {264, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {265, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {266, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {267, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {268, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {269, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {270, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {271, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {272, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {273, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {274, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {275, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {276, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {277, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {278, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {279, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {280, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {281, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {282, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {283, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {284, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {285, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {286, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2, 0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {287, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2, 0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {288, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 1, 0x113e, 0x01bd, 0x1174, 0x1175, 0x112c},
      {289, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2, 0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {290, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2, 0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {291, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3, 0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {292, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3, 0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {293, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 3, 0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {294, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5, 0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {295, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {296, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {297, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 5, 0x1254, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {298, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {299, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {300, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {301, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {302, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 5, 0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {303, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5, 0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {304, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {305, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {306, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {307, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5, 0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {308, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 6, 0x113c, 0xffff, 0x1064, 0x1065, 0x1018},
      {309, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {310, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {311, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {312, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {313, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0x01aa, 0x025c, 0x025d, 0x0210},
      {314, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {315, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {316, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {317, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {318, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {319, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {320, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {321, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {322, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {323, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {324, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {325, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {326, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {327, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {328, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {329, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {330, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {331, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {332, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {333, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {334, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {335, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {336, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {337, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {338, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {339, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {340, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {341, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {342, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {343, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {344, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {345, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {346, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {347, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cb, 0x025c, 0x025d, 0x0210},
      {348, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {349, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {350, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cb, 0x025c, 0x025d, 0x0210},
      {351, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {352, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {353, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {354, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 8, 0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {355, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {356, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {357, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {358, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9, 0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {359, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {360, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8, 0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {361, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {362, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9, 0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {363, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9, 0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {364, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {365, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {366, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {367, LIBRAW_FORMAT_1div2p3INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {368, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {369, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
  };
  ilm.CamID = id;

  if (id == 2)
  {
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    imSony.SonyCameraType = LIBRAW_SONY_DSC;
    imSony.group2010 = 0;
    imSony.real_iso_offset = 0xffff;
    imSony.ImageCount3_offset = 0xffff;
    return;
  }
  else
    idx = id - 256;

  if ((idx >= 0) && (idx < sizeof SonyCamFeatures / sizeof *SonyCamFeatures))
  {
    if (!SonyCamFeatures[idx].scf[2])
      return;
    ilm.CameraFormat = SonyCamFeatures[idx].scf[1];
    ilm.CameraMount = SonyCamFeatures[idx].scf[2];
    imSony.SonyCameraType = SonyCamFeatures[idx].scf[3];
    if (SonyCamFeatures[idx].scf[4])
      ilm.LensMount = SonyCamFeatures[idx].scf[4];
    imSony.group2010 = SonyCamFeatures[idx].scf[5];
    imSony.real_iso_offset = SonyCamFeatures[idx].scf[6];
    imSony.ImageCount3_offset = SonyCamFeatures[idx].scf[7];
    imSony.MeteringMode_offset = SonyCamFeatures[idx].scf[8];
    imSony.ExposureProgram_offset = SonyCamFeatures[idx].scf[9];
    imSony.ReleaseMode2_offset = SonyCamFeatures[idx].scf[10];
  }

  char *sbstr = strstr(software, " v");
  if (sbstr != NULL)
  {
    sbstr += 2;
    imSony.firmware = atof(sbstr);

    if ((id == 306) || (id == 311))
    {
      if (imSony.firmware < 1.2f)
        imSony.ImageCount3_offset = 0x01aa;
      else
        imSony.ImageCount3_offset = 0x01c0;
    }
    else if (id == 312)
    {
      if (imSony.firmware < 2.0f)
        imSony.ImageCount3_offset = 0x01aa;
      else
        imSony.ImageCount3_offset = 0x01c0;
    }
    else if ((id == 318) || (id == 340))
    {
      if (imSony.firmware < 1.2f)
        imSony.ImageCount3_offset = 0x01a0;
      else
        imSony.ImageCount3_offset = 0x01b6;
    }
  }
}