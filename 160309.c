void LibRaw::cam_xyz_coeff(float _rgb_cam[3][4], double cam_xyz[4][3])
{
  double cam_rgb[4][3], inverse[4][3], num;
  int i, j, k;

  for (i = 0; i < colors; i++) /* Multiply out XYZ colorspace */
    for (j = 0; j < 3; j++)
      for (cam_rgb[i][j] = k = 0; k < 3; k++)
        cam_rgb[i][j] += cam_xyz[i][k] * LibRaw_constants::xyz_rgb[k][j];

  for (i = 0; i < colors; i++)
  {                               /* Normalize cam_rgb so that */
    for (num = j = 0; j < 3; j++) /* cam_rgb * (1,1,1) is (1,1,1,1) */
      num += cam_rgb[i][j];
    if (num > 0.00001)
    {
      for (j = 0; j < 3; j++)
        cam_rgb[i][j] /= num;
      pre_mul[i] = 1 / num;
    }
    else
    {
      for (j = 0; j < 3; j++)
        cam_rgb[i][j] = 0.0;
      pre_mul[i] = 1.0;
    }
  }
  pseudoinverse(cam_rgb, inverse, colors);
  for (i = 0; i < 3; i++)
    for (j = 0; j < colors; j++)
      _rgb_cam[i][j] = inverse[j][i];
}