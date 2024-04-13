  int inline Y(ushort3 &rgb) throw()
  {
    return yuv_cam[0][0] * rgb[0] + yuv_cam[0][1] * rgb[1] +
           yuv_cam[0][2] * rgb[2];
  }