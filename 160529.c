void LibRaw::parse_kyocera()
{

  int c;
  static const ushort table[13] = {25,  32,  40,  50,  64,  80, 100,
                                   125, 160, 200, 250, 320, 400};

  fseek(ifp, 33, SEEK_SET);
  get_timestamp(1);
  fseek(ifp, 52, SEEK_SET);
  c = get4();
  if ((c > 6) && (c < 20))
    iso_speed = table[c - 7];
  shutter = libraw_powf64l(2.0f, (((float)get4()) / 8.0f)) / 16000.0f;
  FORC4 cam_mul[c ^ (c >> 1)] = get4();
  fseek(ifp, 88, SEEK_SET);
  aperture = libraw_powf64l(2.0f, ((float)get4()) / 16.0f);
  fseek(ifp, 112, SEEK_SET);
  focal_len = get4();

  fseek(ifp, 104, SEEK_SET);
  ilm.MaxAp4CurFocal = libraw_powf64l(2.0f, ((float)get4()) / 16.0f);
  fseek(ifp, 124, SEEK_SET);
  stmread(ilm.Lens, 32, ifp);
  ilm.CameraMount = LIBRAW_MOUNT_Contax_N;
  ilm.CameraFormat = LIBRAW_FORMAT_FF;
  if (ilm.Lens[0])
  {
    ilm.LensMount = LIBRAW_MOUNT_Contax_N;
    ilm.LensFormat = LIBRAW_FORMAT_FF;
  }
}