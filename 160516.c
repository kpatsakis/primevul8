void LibRaw::bad_pixels(const char *cfname)
{
  FILE *fp = NULL;
  char *cp, line[128];
  int time, row, col, r, c, rad, tot, n;

  if (!filters)
    return;
  RUN_CALLBACK(LIBRAW_PROGRESS_BAD_PIXELS, 0, 2);
  if (cfname)
    fp = fopen(cfname, "r");
  if (!fp)
  {
    imgdata.process_warnings |= LIBRAW_WARN_NO_BADPIXELMAP;
    return;
  }
  while (fgets(line, 128, fp))
  {
    cp = strchr(line, '#');
    if (cp)
      *cp = 0;
    if (sscanf(line, "%d %d %d", &col, &row, &time) != 3)
      continue;
    if ((unsigned)col >= width || (unsigned)row >= height)
      continue;
    if (time > timestamp)
      continue;
    for (tot = n = 0, rad = 1; rad < 3 && n == 0; rad++)
      for (r = row - rad; r <= row + rad; r++)
        for (c = col - rad; c <= col + rad; c++)
          if ((unsigned)r < height && (unsigned)c < width &&
              (r != row || c != col) && fcol(r, c) == fcol(row, col))
          {
            tot += BAYER2(r, c);
            n++;
          }
    if (n > 0)
      BAYER2(row, col) = tot / n;
  }
  fclose(fp);
  RUN_CALLBACK(LIBRAW_PROGRESS_BAD_PIXELS, 1, 2);
}