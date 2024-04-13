void LibRaw::fbdd_green()
{
  int row, col, c, u = width, v = 2 * u, w = 3 * u, x = 4 * u, y = 5 * u, indx,
                   min, max, current;
  float f[4], g[4];

  for (row = 5; row < height - 5; row++)
    for (col = 5 + (FC(row, 1) & 1), indx = row * width + col, c = FC(row, col);
         col < u - 5; col += 2, indx += 2)
    {

      f[0] = 1.0 / (1.0 + abs(image[indx - u][1] - image[indx - w][1]) +
                    abs(image[indx - w][1] - image[indx + y][1]));
      f[1] = 1.0 / (1.0 + abs(image[indx + 1][1] - image[indx + 3][1]) +
                    abs(image[indx + 3][1] - image[indx - 5][1]));
      f[2] = 1.0 / (1.0 + abs(image[indx - 1][1] - image[indx - 3][1]) +
                    abs(image[indx - 3][1] - image[indx + 5][1]));
      f[3] = 1.0 / (1.0 + abs(image[indx + u][1] - image[indx + w][1]) +
                    abs(image[indx + w][1] - image[indx - y][1]));

      g[0] = CLIP((23 * image[indx - u][1] + 23 * image[indx - w][1] +
                   2 * image[indx - y][1] +
                   8 * (image[indx - v][c] - image[indx - x][c]) +
                   40 * (image[indx][c] - image[indx - v][c])) /
                  48.0);
      g[1] = CLIP((23 * image[indx + 1][1] + 23 * image[indx + 3][1] +
                   2 * image[indx + 5][1] +
                   8 * (image[indx + 2][c] - image[indx + 4][c]) +
                   40 * (image[indx][c] - image[indx + 2][c])) /
                  48.0);
      g[2] = CLIP((23 * image[indx - 1][1] + 23 * image[indx - 3][1] +
                   2 * image[indx - 5][1] +
                   8 * (image[indx - 2][c] - image[indx - 4][c]) +
                   40 * (image[indx][c] - image[indx - 2][c])) /
                  48.0);
      g[3] = CLIP((23 * image[indx + u][1] + 23 * image[indx + w][1] +
                   2 * image[indx + y][1] +
                   8 * (image[indx + v][c] - image[indx + x][c]) +
                   40 * (image[indx][c] - image[indx + v][c])) /
                  48.0);

      image[indx][1] =
          CLIP((f[0] * g[0] + f[1] * g[1] + f[2] * g[2] + f[3] * g[3]) /
               (f[0] + f[1] + f[2] + f[3]));

      min = MIN(
          image[indx + 1 + u][1],
          MIN(image[indx + 1 - u][1],
              MIN(image[indx - 1 + u][1],
                  MIN(image[indx - 1 - u][1],
                      MIN(image[indx - 1][1],
                          MIN(image[indx + 1][1],
                              MIN(image[indx - u][1], image[indx + u][1])))))));

      max = MAX(
          image[indx + 1 + u][1],
          MAX(image[indx + 1 - u][1],
              MAX(image[indx - 1 + u][1],
                  MAX(image[indx - 1 - u][1],
                      MAX(image[indx - 1][1],
                          MAX(image[indx + 1][1],
                              MAX(image[indx - u][1], image[indx + u][1])))))));

      image[indx][1] = ULIM(image[indx][1], max, min);
    }
}