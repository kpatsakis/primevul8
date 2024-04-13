void LibRaw::parse_smal(int offset, int fsize)
{
  int ver;

  fseek(ifp, offset + 2, SEEK_SET);
  order = 0x4949;
  ver = fgetc(ifp);
  if (ver == 6)
    fseek(ifp, 5, SEEK_CUR);
  if (get4() != fsize)
    return;
  if (ver > 6)
    data_offset = get4();
  raw_height = height = get2();
  raw_width = width = get2();
  strcpy(make, "SMaL");
  sprintf(model, "v%d %dx%d", ver, width, height);
  if (ver == 6)
    load_raw = &LibRaw::smal_v6_load_raw;
  if (ver == 9)
    load_raw = &LibRaw::smal_v9_load_raw;
}