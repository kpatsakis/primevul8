void LibRaw::dcb_map()
{
  int current, row, col, c, u = width, v = 2 * u, indx;

  for (row = 1; row < height - 1; row++)
  {
    for (col = 1, indx = row * width + col; col < width - 1; col++, indx++)
    {

      if (image[indx][1] > (image[indx - 1][1] + image[indx + 1][1] +
                            image[indx - u][1] + image[indx + u][1]) /
                               4.0)
        image[indx][3] = ((MIN(image[indx - 1][1], image[indx + 1][1]) +
                           image[indx - 1][1] + image[indx + 1][1]) <
                          (MIN(image[indx - u][1], image[indx + u][1]) +
                           image[indx - u][1] + image[indx + u][1]));
      else
        image[indx][3] = ((MAX(image[indx - 1][1], image[indx + 1][1]) +
                           image[indx - 1][1] + image[indx + 1][1]) >
                          (MAX(image[indx - u][1], image[indx + u][1]) +
                           image[indx - u][1] + image[indx + u][1]));
    }
  }
}