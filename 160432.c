void LibRaw::dcb_nyquist()
{
  int row, col, c, u = width, v = 2 * u, indx;

  for (row = 2; row < height - 2; row++)
    for (col = 2 + (FC(row, 2) & 1), indx = row * width + col, c = FC(row, col);
         col < u - 2; col += 2, indx += 2)
    {

      image[indx][1] = CLIP((image[indx + v][1] + image[indx - v][1] +
                             image[indx - 2][1] + image[indx + 2][1]) /
                                4.0 +
                            image[indx][c] -
                            (image[indx + v][c] + image[indx - v][c] +
                             image[indx - 2][c] + image[indx + 2][c]) /
                                4.0);
    }
}