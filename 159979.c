void LibRaw::parse_rollei()
{
  char line[128], *val;
  struct tm t;

  fseek(ifp, 0, SEEK_SET);
  memset(&t, 0, sizeof t);
  do
  {
    line[0] = 0;
    if (!fgets(line, 128, ifp))
      break;
    if(!line[0]) break; // zero-length
    if ((val = strchr(line, '=')))
      *val++ = 0;
    else
      val = line + strbuflen(line);
    if (!strcmp(line, "DAT"))
      sscanf(val, "%d.%d.%d", &t.tm_mday, &t.tm_mon, &t.tm_year);
    if (!strcmp(line, "TIM"))
      sscanf(val, "%d:%d:%d", &t.tm_hour, &t.tm_min, &t.tm_sec);
    if (!strcmp(line, "HDR"))
      thumb_offset = atoi(val);
    if (!strcmp(line, "X  "))
      raw_width = atoi(val);
    if (!strcmp(line, "Y  "))
      raw_height = atoi(val);
    if (!strcmp(line, "TX "))
      thumb_width = atoi(val);
    if (!strcmp(line, "TY "))
      thumb_height = atoi(val);
    if (!strcmp(line, "APT"))
      aperture = atof(val);
    if (!strcmp(line, "SPE"))
      shutter = atof(val);
    if (!strcmp(line, "FOCLEN"))
      focal_len = atof(val);
    if (!strcmp(line, "BLKOFS"))
      black = atoi(val) +1;
    if (!strcmp(line, "ORI"))
      switch (atoi(val)) {
      case 1:
        flip = 6;
        break;
      case 2:
        flip = 3;
        break;
      case 3:
        flip = 5;
        break;
      }
    if (!strcmp(line, "CUTRECT")) {
      sscanf(val, "%hu %hu %hu %hu",
             &imgdata.sizes.raw_inset_crop.cleft,
             &imgdata.sizes.raw_inset_crop.ctop,
             &imgdata.sizes.raw_inset_crop.cwidth,
             &imgdata.sizes.raw_inset_crop.cheight);
    }
  } while (strncmp(line, "EOHD", 4));
  data_offset = thumb_offset + thumb_width * thumb_height * 2;
  t.tm_year -= 1900;
  t.tm_mon -= 1;
  if (mktime(&t) > 0)
    timestamp = mktime(&t);
  strcpy(make, "Rollei");
  strcpy(model, "d530flex");
  write_thumb = &LibRaw::rollei_thumb;
}