void LibRaw::dcb_refinement()
{
  int row, col, c, u = width, v = 2 * u, w = 3 * u, indx, current;
  float f[5], g1, g2, tmp, tmp2 = 0, tmp3 = 0;

  for (row = 4; row < height - 4; row++)
    for (col = 4 + (FC(row, 2) & 1), indx = row * width + col, c = FC(row, col);
         col < u - 4; col += 2, indx += 2)
    {

      current = 4 * image[indx][3] +
                2 * (image[indx + u][3] + image[indx - u][3] +
                     image[indx + 1][3] + image[indx - 1][3]) +
                image[indx + v][3] + image[indx - v][3] + image[indx - 2][3] +
                image[indx + 2][3];

      if (image[indx][c] > 1)
      {

        f[0] = (float)(image[indx - u][1] + image[indx + u][1]) /
               (2 * image[indx][c]);

        if (image[indx - v][c] > 0)
          f[1] = 2 * (float)image[indx - u][1] /
                 (image[indx - v][c] + image[indx][c]);
        else
          f[1] = f[0];

        if (image[indx - v][c] > 0)
          f[2] = (float)(image[indx - u][1] + image[indx - w][1]) /
                 (2 * image[indx - v][c]);
        else
          f[2] = f[0];

        if (image[indx + v][c] > 0)
          f[3] = 2 * (float)image[indx + u][1] /
                 (image[indx + v][c] + image[indx][c]);
        else
          f[3] = f[0];

        if (image[indx + v][c] > 0)
          f[4] = (float)(image[indx + u][1] + image[indx + w][1]) /
                 (2 * image[indx + v][c]);
        else
          f[4] = f[0];

        g1 = (5 * f[0] + 3 * f[1] + f[2] + 3 * f[3] + f[4]) / 13.0;

        f[0] = (float)(image[indx - 1][1] + image[indx + 1][1]) /
               (2 * image[indx][c]);

        if (image[indx - 2][c] > 0)
          f[1] = 2 * (float)image[indx - 1][1] /
                 (image[indx - 2][c] + image[indx][c]);
        else
          f[1] = f[0];

        if (image[indx - 2][c] > 0)
          f[2] = (float)(image[indx - 1][1] + image[indx - 3][1]) /
                 (2 * image[indx - 2][c]);
        else
          f[2] = f[0];

        if (image[indx + 2][c] > 0)
          f[3] = 2 * (float)image[indx + 1][1] /
                 (image[indx + 2][c] + image[indx][c]);
        else
          f[3] = f[0];

        if (image[indx + 2][c] > 0)
          f[4] = (float)(image[indx + 1][1] + image[indx + 3][1]) /
                 (2 * image[indx + 2][c]);
        else
          f[4] = f[0];

        g2 = (5 * f[0] + 3 * f[1] + f[2] + 3 * f[3] + f[4]) / 13.0;

        image[indx][1] = CLIP((image[indx][c]) *
                              (current * g1 + (16 - current) * g2) / 16.0);
      }
      else
        image[indx][1] = image[indx][c];

      // get rid of overshooted pixels

      g1 = MIN(
          image[indx + 1 + u][1],
          MIN(image[indx + 1 - u][1],
              MIN(image[indx - 1 + u][1],
                  MIN(image[indx - 1 - u][1],
                      MIN(image[indx - 1][1],
                          MIN(image[indx + 1][1],
                              MIN(image[indx - u][1], image[indx + u][1])))))));

      g2 = MAX(
          image[indx + 1 + u][1],
          MAX(image[indx + 1 - u][1],
              MAX(image[indx - 1 + u][1],
                  MAX(image[indx - 1 - u][1],
                      MAX(image[indx - 1][1],
                          MAX(image[indx + 1][1],
                              MAX(image[indx - u][1], image[indx + u][1])))))));

      image[indx][1] = ULIM(image[indx][1], g2, g1);
    }
}