void LibRaw::dcb_ver(float (*image3)[3])
{
  int row, col, u = width, v = 2 * u, indx;

  for (row = 2; row < height - 2; row++)
    for (col = 2 + (FC(row, 2) & 1), indx = row * width + col; col < u - 2;
         col += 2, indx += 2)
    {

      image3[indx][1] = CLIP((image[indx + u][1] + image[indx - u][1]) / 2.0);
    }
}