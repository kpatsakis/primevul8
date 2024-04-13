void LibRaw::cielab(ushort rgb[3], short lab[3])
{
  int c, i, j, k;
  float r, xyz[3];
#ifdef LIBRAW_NOTHREADS
  static float cbrt[0x10000], xyz_cam[3][4];
#else
#define cbrt tls->ahd_data.cbrt
#define xyz_cam tls->ahd_data.xyz_cam
#endif

  if (!rgb)
  {
#ifndef LIBRAW_NOTHREADS
    if (cbrt[0] < -1.0f)
#endif
      for (i = 0; i < 0x10000; i++)
      {
        r = i / 65535.0;
        cbrt[i] =
            r > 0.008856 ? pow(r, 1.f / 3.0f) : 7.787f * r + 16.f / 116.0f;
      }
    for (i = 0; i < 3; i++)
      for (j = 0; j < colors; j++)
        for (xyz_cam[i][j] = k = 0; k < 3; k++)
          xyz_cam[i][j] += LibRaw_constants::xyz_rgb[i][k] * rgb_cam[k][j] /
                           LibRaw_constants::d65_white[i];
    return;
  }
  xyz[0] = xyz[1] = xyz[2] = 0.5;
  FORCC
  {
    xyz[0] += xyz_cam[0][c] * rgb[c];
    xyz[1] += xyz_cam[1][c] * rgb[c];
    xyz[2] += xyz_cam[2][c] * rgb[c];
  }
  xyz[0] = cbrt[CLIP((int)xyz[0])];
  xyz[1] = cbrt[CLIP((int)xyz[1])];
  xyz[2] = cbrt[CLIP((int)xyz[2])];
  lab[0] = 64 * (116 * xyz[1] - 16);
  lab[1] = 64 * 500 * (xyz[0] - xyz[1]);
  lab[2] = 64 * 200 * (xyz[1] - xyz[2]);
#ifndef LIBRAW_NOTHREADS
#undef cbrt
#undef xyz_cam
#endif
}