  int inline V(ushort3 &rgb) throw()
  {
    return yuv_cam[2][0] * rgb[0] + yuv_cam[2][1] * rgb[1] +
           yuv_cam[2][2] * rgb[2];
  }