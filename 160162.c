void LibRaw::fbdd_correction2(double (*image2)[3])
{
  int indx, u = width, v = 2 * width;
  int col, row;
  double Co, Ho, ratio;

  for (row = 6; row < height - 6; row++)
  {
    for (col = 6; col < width - 6; col++)
    {
      indx = row * width + col;

      if (image2[indx][1] * image2[indx][2] != 0)
      {
        Co = (image2[indx + v][1] + image2[indx - v][1] + image2[indx - 2][1] +
              image2[indx + 2][1] -
              MAX(image2[indx - 2][1],
                  MAX(image2[indx + 2][1],
                      MAX(image2[indx - v][1], image2[indx + v][1]))) -
              MIN(image2[indx - 2][1],
                  MIN(image2[indx + 2][1],
                      MIN(image2[indx - v][1], image2[indx + v][1])))) /
             2.0;
        Ho = (image2[indx + v][2] + image2[indx - v][2] + image2[indx - 2][2] +
              image2[indx + 2][2] -
              MAX(image2[indx - 2][2],
                  MAX(image2[indx + 2][2],
                      MAX(image2[indx - v][2], image2[indx + v][2]))) -
              MIN(image2[indx - 2][2],
                  MIN(image2[indx + 2][2],
                      MIN(image2[indx - v][2], image2[indx + v][2])))) /
             2.0;
        ratio = sqrt((Co * Co + Ho * Ho) / (image2[indx][1] * image2[indx][1] +
                                            image2[indx][2] * image2[indx][2]));

        if (ratio < 0.85)
        {
          image2[indx][0] =
              -(image2[indx][1] + image2[indx][2] - Co - Ho) + image2[indx][0];
          image2[indx][1] = Co;
          image2[indx][2] = Ho;
        }
      }
    }
  }
}