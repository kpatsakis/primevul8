void LibRaw::Kodak_DCR_WBtags(int wb, unsigned type, int wbi)
{
  float mul[3] = {1.0f, 1.0f, 1.0f}, num, mul2;
  int c;
  FORC3 mul[c] = (num = getreal(type)) <= 0.001f ? 1.0f : num;
  imgdata.color.WB_Coeffs[wb][1] = imgdata.color.WB_Coeffs[wb][3] = mul[1];
  mul2 = mul[1] * mul[1];
  imgdata.color.WB_Coeffs[wb][0] = mul2 / mul[0];
  imgdata.color.WB_Coeffs[wb][2] = mul2 / mul[2];
  if (wbi == wb)
    FORC4 cam_mul[c] = imgdata.color.WB_Coeffs[wb][c];
  return;
}