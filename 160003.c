unsigned LibRaw::get4()
{
  uchar str[4] = {0xff, 0xff, 0xff, 0xff};
  fread(str, 1, 4, ifp);
  return sget4(str);
}