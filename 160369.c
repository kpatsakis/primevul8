void LibRaw::SetStandardIlluminants (unsigned makerIdx, const char* normModel) {
  int i = -1;
  int c;
  if (!imgdata.color.WB_Coeffs[LIBRAW_WBI_Ill_A][0] &&
      !imgdata.color.WB_Coeffs[LIBRAW_WBI_D65][0]) {
    if (makerIdx == LIBRAW_CAMERAMAKER_Olympus) {
      while (++i, imgdata.color.WBCT_Coeffs[i][0]) {
        if (imgdata.color.WBCT_Coeffs[i][0] == 3000)
          FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Ill_A][c] =
                    imgdata.color.WBCT_Coeffs[i][c+1];
        else if (imgdata.color.WBCT_Coeffs[i][0] == 6600)
          FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_D65][c] =
                    imgdata.color.WBCT_Coeffs[i][c+1];
      }
    }
  }

  if (!imgdata.color.WB_Coeffs[LIBRAW_WBI_Ill_A][0] &&
      imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][0])
    FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_Ill_A][c] =
              imgdata.color.WB_Coeffs[LIBRAW_WBI_Tungsten][c];

  if (!imgdata.color.WB_Coeffs[LIBRAW_WBI_D65][0] &&
      imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_N][0])
    FORC4 imgdata.color.WB_Coeffs[LIBRAW_WBI_D65][c] =
              imgdata.color.WB_Coeffs[LIBRAW_WBI_FL_N][c];

  return;
}