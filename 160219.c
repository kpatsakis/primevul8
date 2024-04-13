void LibRaw::Kodak_KDC_WBtags(int wb, int wbi)
{
  int c;
  FORC3 imgdata.color.WB_Coeffs[wb][c] = get4();
  imgdata.color.WB_Coeffs[wb][3] = imgdata.color.WB_Coeffs[wb][1];
  if (wbi == wb)
    FORC4 cam_mul[c] = imgdata.color.WB_Coeffs[wb][c];
  return;
}