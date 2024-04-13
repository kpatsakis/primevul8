void LibRaw::dcb_pp()
{
  int g1, r1, b1, u = width, indx, row, col;

  for (row = 2; row < height - 2; row++)
    for (col = 2, indx = row * u + col; col < width - 2; col++, indx++)
    {

      r1 = (image[indx - 1][0] + image[indx + 1][0] + image[indx - u][0] +
            image[indx + u][0] + image[indx - u - 1][0] +
            image[indx + u + 1][0] + image[indx - u + 1][0] +
            image[indx + u - 1][0]) /
           8.0;
      g1 = (image[indx - 1][1] + image[indx + 1][1] + image[indx - u][1] +
            image[indx + u][1] + image[indx - u - 1][1] +
            image[indx + u + 1][1] + image[indx - u + 1][1] +
            image[indx + u - 1][1]) /
           8.0;
      b1 = (image[indx - 1][2] + image[indx + 1][2] + image[indx - u][2] +
            image[indx + u][2] + image[indx - u - 1][2] +
            image[indx + u + 1][2] + image[indx - u + 1][2] +
            image[indx + u - 1][2]) /
           8.0;

      image[indx][0] = CLIP(r1 + (image[indx][1] - g1));
      image[indx][2] = CLIP(b1 + (image[indx][1] - g1));
    }
}