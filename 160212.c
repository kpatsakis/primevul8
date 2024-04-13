void LibRaw::fbdd_correction()
{
  int row, col, c, u = width, indx;
  ushort(*pix)[4];

  for (row = 2; row < height - 2; row++)
  {
    for (col = 2, indx = row * width + col; col < width - 2; col++, indx++)
    {

      c = fcol(row, col);

      image[indx][c] =
          ULIM(image[indx][c],
               MAX(image[indx - 1][c],
                   MAX(image[indx + 1][c],
                       MAX(image[indx - u][c], image[indx + u][c]))),
               MIN(image[indx - 1][c],
                   MIN(image[indx + 1][c],
                       MIN(image[indx - u][c], image[indx + u][c]))));
    }
  }
}