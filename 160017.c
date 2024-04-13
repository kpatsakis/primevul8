void LibRaw::dcb_decide(float (*image2)[3], float (*image3)[3])
{
  int row, col, c, d, u = width, v = 2 * u, indx;
  float current, current2, current3;

  for (row = 2; row < height - 2; row++)
    for (col = 2 + (FC(row, 2) & 1), indx = row * width + col, c = FC(row, col);
         col < u - 2; col += 2, indx += 2)
    {

      d = ABS(c - 2);

      current = MAX(image[indx + v][c],
                    MAX(image[indx - v][c],
                        MAX(image[indx - 2][c], image[indx + 2][c]))) -
                MIN(image[indx + v][c],
                    MIN(image[indx - v][c],
                        MIN(image[indx - 2][c], image[indx + 2][c]))) +
                MAX(image[indx + 1 + u][d],
                    MAX(image[indx + 1 - u][d],
                        MAX(image[indx - 1 + u][d], image[indx - 1 - u][d]))) -
                MIN(image[indx + 1 + u][d],
                    MIN(image[indx + 1 - u][d],
                        MIN(image[indx - 1 + u][d], image[indx - 1 - u][d])));

      current2 =
          MAX(image2[indx + v][d],
              MAX(image2[indx - v][d],
                  MAX(image2[indx - 2][d], image2[indx + 2][d]))) -
          MIN(image2[indx + v][d],
              MIN(image2[indx - v][d],
                  MIN(image2[indx - 2][d], image2[indx + 2][d]))) +
          MAX(image2[indx + 1 + u][c],
              MAX(image2[indx + 1 - u][c],
                  MAX(image2[indx - 1 + u][c], image2[indx - 1 - u][c]))) -
          MIN(image2[indx + 1 + u][c],
              MIN(image2[indx + 1 - u][c],
                  MIN(image2[indx - 1 + u][c], image2[indx - 1 - u][c])));

      current3 =
          MAX(image3[indx + v][d],
              MAX(image3[indx - v][d],
                  MAX(image3[indx - 2][d], image3[indx + 2][d]))) -
          MIN(image3[indx + v][d],
              MIN(image3[indx - v][d],
                  MIN(image3[indx - 2][d], image3[indx + 2][d]))) +
          MAX(image3[indx + 1 + u][c],
              MAX(image3[indx + 1 - u][c],
                  MAX(image3[indx - 1 + u][c], image3[indx - 1 - u][c]))) -
          MIN(image3[indx + 1 + u][c],
              MIN(image3[indx + 1 - u][c],
                  MIN(image3[indx - 1 + u][c], image3[indx - 1 - u][c])));

      if (ABS(current - current2) < ABS(current - current3))
        image[indx][1] = image2[indx][1];
      else
        image[indx][1] = image3[indx][1];
    }
}