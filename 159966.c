void LibRaw::dcb_color3(float (*image3)[3])
{
  int row, col, c, d, u = width, indx;

  for (row = 1; row < height - 1; row++)
    for (col = 1 + (FC(row, 1) & 1), indx = row * width + col,
        c = 2 - FC(row, col);
         col < u - 1; col += 2, indx += 2)
    {

      image3[indx][c] =
          CLIP((4 * image3[indx][1] - image3[indx + u + 1][1] -
                image3[indx + u - 1][1] - image3[indx - u + 1][1] -
                image3[indx - u - 1][1] + image[indx + u + 1][c] +
                image[indx + u - 1][c] + image[indx - u + 1][c] +
                image[indx - u - 1][c]) /
               4.0);
    }

  for (row = 1; row < height - 1; row++)
    for (col = 1 + (FC(row, 2) & 1), indx = row * width + col,
        c = FC(row, col + 1), d = 2 - c;
         col < width - 1; col += 2, indx += 2)
    {

      image3[indx][c] =
          CLIP((2 * image3[indx][1] - image3[indx + 1][1] -
                image3[indx - 1][1] + image[indx + 1][c] + image[indx - 1][c]) /
               2.0);
      image3[indx][d] = CLIP((image[indx + u][d] + image[indx - u][d]) / 2.0);
    }
}