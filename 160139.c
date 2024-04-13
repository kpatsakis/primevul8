void LibRaw::process_Hassy_Lens (int LensMount) {
// long long unsigned id =
//    mount*100000000ULL + series*10000000ULL +
//    focal1*10000ULL + focal2*10 + version;
  char *ps;
  int c = atoi(strchr(imgdata.lens.Lens, ' ') +1);
  if (!c)
    return;

  if (LensMount == LIBRAW_MOUNT_Hasselblad_H) {
    if (imgdata.lens.Lens[2] == ' ') // HC lens
      ilm.LensID = LensMount*100000000ULL + 10000000ULL;
    else                             // HCD lens
      ilm.LensID = LensMount*100000000ULL + 20000000ULL;
    ilm.LensFormat = LIBRAW_FORMAT_645;
  } else if (LensMount == LIBRAW_MOUNT_Hasselblad_XCD) {
    ilm.LensFormat = LIBRAW_FORMAT_CROP645;
    ilm.LensID = LensMount*100000000ULL;
  } else
    return;

  ilm.LensMount = LensMount;
  ilm.LensID += c*10000ULL;
  if ((ps=strchr(imgdata.lens.Lens, '-'))) {
    ilm.FocalType = LIBRAW_FT_ZOOM_LENS;
    ilm.LensID += atoi(ps+1)*10ULL;
  } else {
    ilm.FocalType = LIBRAW_FT_PRIME_LENS;
    ilm.LensID += c*10ULL;
  }
  if (strstr(imgdata.lens.Lens, "III"))
    ilm.LensID += 3ULL;
  else if (strstr(imgdata.lens.Lens, "II"))
    ilm.LensID += 2ULL;
}