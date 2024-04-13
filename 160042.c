static ushort saneSonyCameraInfo(uchar a, uchar b, uchar c, uchar d, uchar e,
                                 uchar f)
{
  if ((a >> 4) > 9)
    return 0;
  else if ((a & 0x0f) > 9)
    return 0;
  else if ((b >> 4) > 9)
    return 0;
  else if ((b & 0x0f) > 9)
    return 0;
  else if ((c >> 4) > 9)
    return 0;
  else if ((c & 0x0f) > 9)
    return 0;
  else if ((d >> 4) > 9)
    return 0;
  else if ((d & 0x0f) > 9)
    return 0;
  else if ((e >> 4) > 9)
    return 0;
  else if ((e & 0x0f) > 9)
    return 0;
  else if ((f >> 4) > 9)
    return 0;
  else if ((f & 0x0f) > 9)
    return 0;
  return 1;
}