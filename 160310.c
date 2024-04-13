void LibRaw::fuji_rotate()
{
  int i, row, col;
  double step;
  float r, c, fr, fc;
  unsigned ur, uc;
  ushort wide, high, (*img)[4], (*pix)[4];

  if (!fuji_width)
    return;
  fuji_width = (fuji_width - 1 + shrink) >> shrink;
  step = sqrt(0.5);
  wide = fuji_width / step;
  high = (height - fuji_width) / step;
  img = (ushort(*)[4])calloc(high, wide * sizeof *img);
  merror(img, "fuji_rotate()");

  RUN_CALLBACK(LIBRAW_PROGRESS_FUJI_ROTATE, 0, 2);

  for (row = 0; row < high; row++)
    for (col = 0; col < wide; col++)
    {
      ur = r = fuji_width + (row - col) * step;
      uc = c = (row + col) * step;
      if (ur > height - 2 || uc > width - 2)
        continue;
      fr = r - ur;
      fc = c - uc;
      pix = image + ur * width + uc;
      for (i = 0; i < colors; i++)
        img[row * wide + col][i] =
            (pix[0][i] * (1 - fc) + pix[1][i] * fc) * (1 - fr) +
            (pix[width][i] * (1 - fc) + pix[width + 1][i] * fc) * fr;
    }

  free(image);
  width = wide;
  height = high;
  image = img;
  fuji_width = 0;
  RUN_CALLBACK(LIBRAW_PROGRESS_FUJI_ROTATE, 1, 2);
}