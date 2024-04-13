void LibRaw::green_matching()
{
  int i, j;
  double m1, m2, c1, c2;
  int o1_1, o1_2, o1_3, o1_4;
  int o2_1, o2_2, o2_3, o2_4;
  ushort(*img)[4];
  const int margin = 3;
  int oj = 2, oi = 2;
  float f;
  const float thr = 0.01f;
  if (half_size || shrink)
    return;
  if (FC(oj, oi) != 3)
    oj++;
  if (FC(oj, oi) != 3)
    oi++;
  if (FC(oj, oi) != 3)
    oj--;

  img = (ushort(*)[4])calloc(height * width, sizeof *image);
  merror(img, "green_matching()");
  memcpy(img, image, height * width * sizeof *image);

  for (j = oj; j < height - margin; j += 2)
    for (i = oi; i < width - margin; i += 2)
    {
      o1_1 = img[(j - 1) * width + i - 1][1];
      o1_2 = img[(j - 1) * width + i + 1][1];
      o1_3 = img[(j + 1) * width + i - 1][1];
      o1_4 = img[(j + 1) * width + i + 1][1];
      o2_1 = img[(j - 2) * width + i][3];
      o2_2 = img[(j + 2) * width + i][3];
      o2_3 = img[j * width + i - 2][3];
      o2_4 = img[j * width + i + 2][3];

      m1 = (o1_1 + o1_2 + o1_3 + o1_4) / 4.0;
      m2 = (o2_1 + o2_2 + o2_3 + o2_4) / 4.0;

      c1 = (abs(o1_1 - o1_2) + abs(o1_1 - o1_3) + abs(o1_1 - o1_4) +
            abs(o1_2 - o1_3) + abs(o1_3 - o1_4) + abs(o1_2 - o1_4)) /
           6.0;
      c2 = (abs(o2_1 - o2_2) + abs(o2_1 - o2_3) + abs(o2_1 - o2_4) +
            abs(o2_2 - o2_3) + abs(o2_3 - o2_4) + abs(o2_2 - o2_4)) /
           6.0;
      if ((img[j * width + i][3] < maximum * 0.95) && (c1 < maximum * thr) &&
          (c2 < maximum * thr))
      {
        f = image[j * width + i][3] * m1 / m2;
        image[j * width + i][3] = f > 0xffff ? 0xffff : f;
      }
    }
  free(img);
}