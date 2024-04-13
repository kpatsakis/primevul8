void LibRaw::setSonyBodyFeatures(unsigned long long id)
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
    scf[7]  offset of ShutterCount3 in 0x9050 table, 0xffff if not valid
    scf[8]  offset of MeteringMode in 0x2010 table, 0xffff if not valid
    scf[9]  offset of ExposureProgram in 0x2010 table, 0xffff if not valid
    scf[10] offset of ReleaseMode2 in 0x2010 table, 0xffff if not valid
    */
  } SonyCamFeatures[] = {
      {0x100, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x101, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x102, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x103, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x104, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x105, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x106, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x107, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x108, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x109, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10a, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10b, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10d, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10e, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x10f, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x110, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x111, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x112, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x113, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x114, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x115, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x116, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x117, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x118, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x119, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x11a, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x11b, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_DSLR, 0,
       0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x11c, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x11d, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 0,
       0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x11e, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {0x11f, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {0x120, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 1,
       0x113e, 0x01bd, 0x1174, 0x1175, 0x112c},
      {0x121, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {0x122, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 2,
       0x1218, 0x01bd, 0x1178, 0x1179, 0x112c},
      {0x123, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3,
       0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {0x124, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 3,
       0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {0x125, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 3,
       0x11f4, 0x01bd, 0x1154, 0x1155, 0x1108},
      {0x126, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x127, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x128, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x129, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 5, 0x1254, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {0x12a, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {0x12b, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x12c, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x12d, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x12e, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 5,
       0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x12f, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_SLT, 0, 5,
       0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x130, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x131, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1280, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x132, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x133, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_NEX, 0, 5,
       0x1254, 0x01aa, 0x11ac, 0x11ad, 0x1160},
      {0x134, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 6, 0x113c, 0xffff, 0x1064, 0x1065, 0x1018},
      {0x135, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x136, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 5, 0x1258, 0xffff, 0x11ac, 0x11ad, 0x1160},
      {0x137, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x138, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x139, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0x01aa, 0x025c, 0x025d, 0x0210},
      {0x13a, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x13b, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x13c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x13d, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 7, 0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x13e, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x13f, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0,
       7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {0x140, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x141, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x142, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x143, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x144, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x145, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x146, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x147, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x148, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x149, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14a, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14b, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14d, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14e, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x14f, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x150, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x151, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x152, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x153, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {0x154, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x155, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x156, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x157, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x158, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x159, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x15a, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 7,
       0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {0x15b, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cb, 0x025c, 0x025d, 0x0210},
      {0x15c, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x15d, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x15e, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cb, 0x025c, 0x025d, 0x0210},
      {0x15f, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x160, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x161, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0,
       7, 0x0344, 0x01a0, 0x025c, 0x025d, 0x0210},
      {0x162, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Minolta_A, LIBRAW_SONY_ILCA, 0, 8,
       0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {0x163, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x164, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x165, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {0x166, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x167, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x168, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 8,
       0x0346, 0x01cd, 0x025c, 0x025d, 0x0210},
      {0x169, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x16a, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x16b, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x16c, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 8, 0x0346, 0xffff, 0x025c, 0x025d, 0x0210},
      {0x16d, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x16e, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x16f, LIBRAW_FORMAT_1div2p3INCH, LIBRAW_MOUNT_FixedLens,
       LIBRAW_SONY_DSC, LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b,
       0x024c, 0x0208},
      {0x170, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x171, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x172, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x173, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x174, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x175, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x176, LIBRAW_FORMAT_1INCH, LIBRAW_MOUNT_FixedLens, LIBRAW_SONY_DSC,
       LIBRAW_MOUNT_FixedLens, 9, 0x0320, 0xffff, 0x024b, 0x024c, 0x0208},
      {0x177, LIBRAW_FORMAT_FF, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x178, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x179, 0, 0, 0, 0, 0, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff},
      {0x17a, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},
      {0x17b, LIBRAW_FORMAT_APSC, LIBRAW_MOUNT_Sony_E, LIBRAW_SONY_ILCE, 0, 9,
       0x0320, 0x019f, 0x024b, 0x024c, 0x0208},

  };
  ilm.CamID = id;

  if (id == 0x002ULL)
  {
    ilm.CameraMount = ilm.LensMount = LIBRAW_MOUNT_FixedLens;
    imSony.CameraType = LIBRAW_SONY_DSC;
    imSony.group2010 = 0;
    imSony.real_iso_offset = 0xffff;
    imSony.ImageCount3_offset = 0xffff;
    return;
  }
  else
    idx = id - 0x100ULL;

  if ((idx >= 0) && (idx < sizeof SonyCamFeatures / sizeof *SonyCamFeatures))
  {
    if (!SonyCamFeatures[idx].scf[2])
      return;
    ilm.CameraFormat = SonyCamFeatures[idx].scf[1];
    ilm.CameraMount = SonyCamFeatures[idx].scf[2];
    imSony.CameraType = SonyCamFeatures[idx].scf[3];
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

    if ((id == 0x132ULL) || // ILCE-7
        (id == 0x137ULL))
    { // ILCE-7R
      if (imSony.firmware < 1.2f)
        imSony.ImageCount3_offset = 0x01aa;
      else
        imSony.ImageCount3_offset = 0x01c0;
    }
    else if (id == 0x138ULL)
    { // ILCE-6000
      if (imSony.firmware < 2.0f)
        imSony.ImageCount3_offset = 0x01aa;
      else
        imSony.ImageCount3_offset = 0x01c0;
    }
    else if ((id == 0x13eULL) || // ILCE-7S
             (id == 0x154ULL))
    { // ILCE-7M2
      if (imSony.firmware < 1.2f)
        imSony.ImageCount3_offset = 0x01a0;
      else
        imSony.ImageCount3_offset = 0x01b6;
    }
  }
}