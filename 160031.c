  int inline U(ushort3 &rgb) throw()
  {
    return yuv_cam[1][0] * rgb[0] + yuv_cam[1][1] * rgb[1] +
           yuv_cam[1][2] * rgb[2];
  }