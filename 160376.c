void LibRaw::dcb_correction()
{
  int current, row, col, u = width, v = 2 * u, indx;

  for (row = 2; row < height - 2; row++)
    for (col = 2 + (FC(row, 2) & 1), indx = row * width + col; col < u - 2;
         col += 2, indx += 2)
    {

      current = 4 * image[indx][3] +
                2 * (image[indx + u][3] + image[indx - u][3] +
                     image[indx + 1][3] + image[indx - 1][3]) +
                image[indx + v][3] + image[indx - v][3] + image[indx + 2][3] +
                image[indx - 2][3];

      image[indx][1] =
          ((16 - current) * (image[indx - 1][1] + image[indx + 1][1]) / 2.0 +
           current * (image[indx - u][1] + image[indx + u][1]) / 2.0) /
          16.0;
    }
}