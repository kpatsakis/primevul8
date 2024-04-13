void tiff_head(int width, int height, struct tiff_hdr *th)
{
  int c;
  time_t timestamp = time(NULL);
  struct tm *t;

  memset(th, 0, sizeof *th);
  th->t_order = htonl(0x4d4d4949) >> 16;
  th->magic = 42;
  th->ifd = 10;
  tiff_set(&th->ntag, 254, 4, 1, 0);
  tiff_set(&th->ntag, 256, 4, 1, width);
  tiff_set(&th->ntag, 257, 4, 1, height);
  tiff_set(&th->ntag, 258, 3, 1, 16);
  for (c = 0; c < 4; c++)
    th->bps[c] = 16;
  tiff_set(&th->ntag, 259, 3, 1, 1);
  tiff_set(&th->ntag, 262, 3, 1, 1);
  tiff_set(&th->ntag, 273, 4, 1, sizeof *th);
  tiff_set(&th->ntag, 277, 3, 1, 1);
  tiff_set(&th->ntag, 278, 4, 1, height);
  tiff_set(&th->ntag, 279, 4, 1, height * width * 2);
  tiff_set(&th->ntag, 282, 5, 1, TOFF(th->rat[0]));
  tiff_set(&th->ntag, 283, 5, 1, TOFF(th->rat[2]));
  tiff_set(&th->ntag, 284, 3, 1, 1);
  tiff_set(&th->ntag, 296, 3, 1, 2);
  tiff_set(&th->ntag, 306, 2, 20, TOFF(th->date));
  th->rat[0] = th->rat[2] = 300;
  th->rat[1] = th->rat[3] = 1;
  t = localtime(&timestamp);
  if (t)
    sprintf(th->date, "%04d:%02d:%02d %02d:%02d:%02d", t->tm_year + 1900,
            t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}