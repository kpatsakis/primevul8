void LibRaw::aRGB_coeff(double aRGB_cam[3][3])
{
  static const double rgb_aRGB[3][3] = {
      {1.39828313770000, -0.3982830047, 9.64980900741708E-8},
      {6.09219200572997E-8, 0.9999999809, 1.33230799934103E-8},
      {2.17237099975343E-8, -0.0429383201, 1.04293828050000}};

  double cmatrix_tmp[3][3] = {
      {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}};
  int i, j, k;

  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++)
    {
      for (k = 0; k < 3; k++)
        cmatrix_tmp[i][j] += rgb_aRGB[i][k] * aRGB_cam[k][j];
      cmatrix[i][j] = (float)cmatrix_tmp[i][j];
    }
}