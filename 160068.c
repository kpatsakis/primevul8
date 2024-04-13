void LibRaw::dcb_color_full()
{
  int row, col, c, d, i, j, u = width, v = 2 * u, w = 3 * u, indx, g1, g2;
  float f[4], g[4], (*chroma)[2];

  chroma = (float(*)[2])calloc(width * height, sizeof *chroma);
  merror(chroma, "dcb_color_full()");

  for (row = 1; row < height - 1; row++)
    for (col = 1 + (FC(row, 1) & 1), indx = row * width + col, c = FC(row, col),
        d = c / 2;
         col < u - 1; col += 2, indx += 2)
      chroma[indx][d] = image[indx][c] - image[indx][1];

  for (row = 3; row < height - 3; row++)
    for (col = 3 + (FC(row, 1) & 1), indx = row * width + col,
        c = 1 - FC(row, col) / 2, d = 1 - c;
         col < u - 3; col += 2, indx += 2)
    {
      f[0] = 1.0 /
             (float)(1.0 +
                     fabs(chroma[indx - u - 1][c] - chroma[indx + u + 1][c]) +
                     fabs(chroma[indx - u - 1][c] - chroma[indx - w - 3][c]) +
                     fabs(chroma[indx + u + 1][c] - chroma[indx - w - 3][c]));
      f[1] = 1.0 /
             (float)(1.0 +
                     fabs(chroma[indx - u + 1][c] - chroma[indx + u - 1][c]) +
                     fabs(chroma[indx - u + 1][c] - chroma[indx - w + 3][c]) +
                     fabs(chroma[indx + u - 1][c] - chroma[indx - w + 3][c]));
      f[2] = 1.0 /
             (float)(1.0 +
                     fabs(chroma[indx + u - 1][c] - chroma[indx - u + 1][c]) +
                     fabs(chroma[indx + u - 1][c] - chroma[indx + w + 3][c]) +
                     fabs(chroma[indx - u + 1][c] - chroma[indx + w - 3][c]));
      f[3] = 1.0 /
             (float)(1.0 +
                     fabs(chroma[indx + u + 1][c] - chroma[indx - u - 1][c]) +
                     fabs(chroma[indx + u + 1][c] - chroma[indx + w - 3][c]) +
                     fabs(chroma[indx - u - 1][c] - chroma[indx + w + 3][c]));
      g[0] = 1.325 * chroma[indx - u - 1][c] - 0.175 * chroma[indx - w - 3][c] -
             0.075 * chroma[indx - w - 1][c] - 0.075 * chroma[indx - u - 3][c];
      g[1] = 1.325 * chroma[indx - u + 1][c] - 0.175 * chroma[indx - w + 3][c] -
             0.075 * chroma[indx - w + 1][c] - 0.075 * chroma[indx - u + 3][c];
      g[2] = 1.325 * chroma[indx + u - 1][c] - 0.175 * chroma[indx + w - 3][c] -
             0.075 * chroma[indx + w - 1][c] - 0.075 * chroma[indx + u - 3][c];
      g[3] = 1.325 * chroma[indx + u + 1][c] - 0.175 * chroma[indx + w + 3][c] -
             0.075 * chroma[indx + w + 1][c] - 0.075 * chroma[indx + u + 3][c];
      chroma[indx][c] =
          (f[0] * g[0] + f[1] * g[1] + f[2] * g[2] + f[3] * g[3]) /
          (f[0] + f[1] + f[2] + f[3]);
    }
  for (row = 3; row < height - 3; row++)
    for (col = 3 + (FC(row, 2) & 1), indx = row * width + col,
        c = FC(row, col + 1) / 2;
         col < u - 3; col += 2, indx += 2)
      for (d = 0; d <= 1; c = 1 - c, d++)
      {
        f[0] = 1.0 /
               (float)(1.0 + fabs(chroma[indx - u][c] - chroma[indx + u][c]) +
                       fabs(chroma[indx - u][c] - chroma[indx - w][c]) +
                       fabs(chroma[indx + u][c] - chroma[indx - w][c]));
        f[1] = 1.0 /
               (float)(1.0 + fabs(chroma[indx + 1][c] - chroma[indx - 1][c]) +
                       fabs(chroma[indx + 1][c] - chroma[indx + 3][c]) +
                       fabs(chroma[indx - 1][c] - chroma[indx + 3][c]));
        f[2] = 1.0 /
               (float)(1.0 + fabs(chroma[indx - 1][c] - chroma[indx + 1][c]) +
                       fabs(chroma[indx - 1][c] - chroma[indx - 3][c]) +
                       fabs(chroma[indx + 1][c] - chroma[indx - 3][c]));
        f[3] = 1.0 /
               (float)(1.0 + fabs(chroma[indx + u][c] - chroma[indx - u][c]) +
                       fabs(chroma[indx + u][c] - chroma[indx + w][c]) +
                       fabs(chroma[indx - u][c] - chroma[indx + w][c]));

        g[0] = 0.875 * chroma[indx - u][c] + 0.125 * chroma[indx - w][c];
        g[1] = 0.875 * chroma[indx + 1][c] + 0.125 * chroma[indx + 3][c];
        g[2] = 0.875 * chroma[indx - 1][c] + 0.125 * chroma[indx - 3][c];
        g[3] = 0.875 * chroma[indx + u][c] + 0.125 * chroma[indx + w][c];

        chroma[indx][c] =
            (f[0] * g[0] + f[1] * g[1] + f[2] * g[2] + f[3] * g[3]) /
            (f[0] + f[1] + f[2] + f[3]);
      }

  for (row = 6; row < height - 6; row++)
    for (col = 6, indx = row * width + col; col < width - 6; col++, indx++)
    {
      image[indx][0] = CLIP(chroma[indx][0] + image[indx][1]);
      image[indx][2] = CLIP(chroma[indx][1] + image[indx][1]);

      g1 = MIN(
          image[indx + 1 + u][0],
          MIN(image[indx + 1 - u][0],
              MIN(image[indx - 1 + u][0],
                  MIN(image[indx - 1 - u][0],
                      MIN(image[indx - 1][0],
                          MIN(image[indx + 1][0],
                              MIN(image[indx - u][0], image[indx + u][0])))))));

      g2 = MAX(
          image[indx + 1 + u][0],
          MAX(image[indx + 1 - u][0],
              MAX(image[indx - 1 + u][0],
                  MAX(image[indx - 1 - u][0],
                      MAX(image[indx - 1][0],
                          MAX(image[indx + 1][0],
                              MAX(image[indx - u][0], image[indx + u][0])))))));

      image[indx][0] = ULIM(image[indx][0], g2, g1);

      g1 = MIN(
          image[indx + 1 + u][2],
          MIN(image[indx + 1 - u][2],
              MIN(image[indx - 1 + u][2],
                  MIN(image[indx - 1 - u][2],
                      MIN(image[indx - 1][2],
                          MIN(image[indx + 1][2],
                              MIN(image[indx - u][2], image[indx + u][2])))))));

      g2 = MAX(
          image[indx + 1 + u][2],
          MAX(image[indx + 1 - u][2],
              MAX(image[indx - 1 + u][2],
                  MAX(image[indx - 1 - u][2],
                      MAX(image[indx - 1][2],
                          MAX(image[indx + 1][2],
                              MAX(image[indx - u][2], image[indx + u][2])))))));

      image[indx][2] = ULIM(image[indx][2], g2, g1);
    }

  free(chroma);
}