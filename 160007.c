void LibRaw::Nikon_NRW_WBtag(int wb, int skip)
{

#define icWB imgdata.color.WB_Coeffs
  int r, g0, g1, b;
  if (skip)
    get4(); // skip wb "CCT", it is not unique
  r = get4();
  g0 = get4();
  g1 = get4();
  b = get4();
  if (r && g0 && g1 && b)
  {
    icWB[wb][0] = r << 1;
    icWB[wb][1] = g0;
    icWB[wb][2] = b << 1;
    icWB[wb][3] = g1;
  }
  return;
#undef icWB
}