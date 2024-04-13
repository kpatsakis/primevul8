void LibRaw::dcb_hor(float (*image2)[3])
{
  int row, col, u = width, v = 2 * u, indx;

  for (row = 2; row < height - 2; row++)
    for (col = 2 + (FC(row, 2) & 1), indx = row * width + col; col < u - 2;
         col += 2, indx += 2)
    {

      image2[indx][1] = CLIP((image[indx + 1][1] + image[indx - 1][1]) / 2.0);
    }
}