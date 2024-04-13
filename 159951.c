void LibRaw::dcb_correction2()
{
  int current, row, col, c, u = width, v = 2 * u, indx;
  ushort(*pix)[4];

  for (row = 4; row < height - 4; row++)
    for (col = 4 + (FC(row, 2) & 1), indx = row * width + col, c = FC(row, col);
         col < u - 4; col += 2, indx += 2)
    {

      current = 4 * image[indx][3] +
                2 * (image[indx + u][3] + image[indx - u][3] +
                     image[indx + 1][3] + image[indx - 1][3]) +
                image[indx + v][3] + image[indx - v][3] + image[indx + 2][3] +
                image[indx - 2][3];

      image[indx][1] = CLIP(
          ((16 - current) * ((image[indx - 1][1] + image[indx + 1][1]) / 2.0 +
                             image[indx][c] -
                             (image[indx + 2][c] + image[indx - 2][c]) / 2.0) +
           current * ((image[indx - u][1] + image[indx + u][1]) / 2.0 +
                      image[indx][c] -
                      (image[indx + v][c] + image[indx - v][c]) / 2.0)) /
          16.0);
    }
}