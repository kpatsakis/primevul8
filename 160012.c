void LibRaw::tiff_head(struct tiff_hdr *th, int full)
{
  int c, psize = 0;
  struct tm *t;

  memset(th, 0, sizeof *th);
  th->t_order = htonl(0x4d4d4949) >> 16;
  th->magic = 42;
  th->ifd = 10;
  th->rat[0] = th->rat[2] = 300;
  th->rat[1] = th->rat[3] = 1;
  FORC(6) th->rat[4 + c] = 1000000;
  th->rat[4] *= shutter;
  th->rat[6] *= aperture;
  th->rat[8] *= focal_len;
  strncpy(th->t_desc, desc, 512);
  strncpy(th->t_make, make, 64);
  strncpy(th->t_model, model, 64);
  strcpy(th->soft, "dcraw v" DCRAW_VERSION);
  t = localtime(&timestamp);
  sprintf(th->date, "%04d:%02d:%02d %02d:%02d:%02d", t->tm_year + 1900,
          t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
  strncpy(th->t_artist, artist, 64);
  if (full)
  {
    tiff_set(th, &th->ntag, 254, 4, 1, 0);
    tiff_set(th, &th->ntag, 256, 4, 1, width);
    tiff_set(th, &th->ntag, 257, 4, 1, height);
    tiff_set(th, &th->ntag, 258, 3, colors, output_bps);
    if (colors > 2)
      th->tag[th->ntag - 1].val.i = TOFF(th->bps);
    FORC4 th->bps[c] = output_bps;
    tiff_set(th, &th->ntag, 259, 3, 1, 1);
    tiff_set(th, &th->ntag, 262, 3, 1, 1 + (colors > 1));
  }
  tiff_set(th, &th->ntag, 270, 2, 512, TOFF(th->t_desc));
  tiff_set(th, &th->ntag, 271, 2, 64, TOFF(th->t_make));
  tiff_set(th, &th->ntag, 272, 2, 64, TOFF(th->t_model));
  if (full)
  {
    if (oprof)
      psize = ntohl(oprof[0]);
    tiff_set(th, &th->ntag, 273, 4, 1, sizeof *th + psize);
    tiff_set(th, &th->ntag, 277, 3, 1, colors);
    tiff_set(th, &th->ntag, 278, 4, 1, height);
    tiff_set(th, &th->ntag, 279, 4, 1,
             height * width * colors * output_bps / 8);
  }
  else
    tiff_set(th, &th->ntag, 274, 3, 1, "12435867"[flip] - '0');
  tiff_set(th, &th->ntag, 282, 5, 1, TOFF(th->rat[0]));
  tiff_set(th, &th->ntag, 283, 5, 1, TOFF(th->rat[2]));
  tiff_set(th, &th->ntag, 284, 3, 1, 1);
  tiff_set(th, &th->ntag, 296, 3, 1, 2);
  tiff_set(th, &th->ntag, 305, 2, 32, TOFF(th->soft));
  tiff_set(th, &th->ntag, 306, 2, 20, TOFF(th->date));
  tiff_set(th, &th->ntag, 315, 2, 64, TOFF(th->t_artist));
  tiff_set(th, &th->ntag, 34665, 4, 1, TOFF(th->nexif));
  if (psize)
    tiff_set(th, &th->ntag, 34675, 7, psize, sizeof *th);
  tiff_set(th, &th->nexif, 33434, 5, 1, TOFF(th->rat[4]));
  tiff_set(th, &th->nexif, 33437, 5, 1, TOFF(th->rat[6]));
  tiff_set(th, &th->nexif, 34855, 3, 1, iso_speed);
  tiff_set(th, &th->nexif, 37386, 5, 1, TOFF(th->rat[8]));
  if (gpsdata[1])
  {
    tiff_set(th, &th->ntag, 34853, 4, 1, TOFF(th->ngps));
    tiff_set(th, &th->ngps, 0, 1, 4, 0x202);
    tiff_set(th, &th->ngps, 1, 2, 2, gpsdata[29]);
    tiff_set(th, &th->ngps, 2, 5, 3, TOFF(th->gps[0]));
    tiff_set(th, &th->ngps, 3, 2, 2, gpsdata[30]);
    tiff_set(th, &th->ngps, 4, 5, 3, TOFF(th->gps[6]));
    tiff_set(th, &th->ngps, 5, 1, 1, gpsdata[31]);
    tiff_set(th, &th->ngps, 6, 5, 1, TOFF(th->gps[18]));
    tiff_set(th, &th->ngps, 7, 5, 3, TOFF(th->gps[12]));
    tiff_set(th, &th->ngps, 18, 2, 12, TOFF(th->gps[20]));
    tiff_set(th, &th->ngps, 29, 2, 12, TOFF(th->gps[23]));
    memcpy(th->gps, gpsdata, sizeof th->gps);
  }
}