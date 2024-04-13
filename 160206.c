int main(int ac, char *av[])
{
  if (ac != 2)
    return 1;
  FILE *in = fopen(av[1], "rb");
  fseek(in, 0, SEEK_END);
  unsigned fsz = ftell(in);
  unsigned char *buffer = (unsigned char *)malloc(fsz);
  if (!buffer)
    return 2;
  fseek(in, 0, SEEK_SET);
  unsigned readb = fread(buffer, 1, fsz, in);
  if (readb != fsz)
    return 3;
  LibRaw rp;
  rp.imgdata.params.output_tiff = 1;
  int ret = rp.open_bayer(buffer, fsz, 640, 480, 0, 0, 0, 0, 0,
                          LIBRAW_OPENBAYER_RGGB, 0, 0, 1400);
  if (ret != LIBRAW_SUCCESS)
    return 4;
  if ((ret = rp.unpack()) != LIBRAW_SUCCESS)
    printf("Unpack error: %d\n", ret);

  if ((ret = rp.dcraw_process()) != LIBRAW_SUCCESS)
    printf("Processing error: %d\n", ret);

  char outfn[256];
  sprintf(outfn, "%s.tif", av[1]);
  if (LIBRAW_SUCCESS != (ret = rp.dcraw_ppm_tiff_writer(outfn)))
    printf("Cannot write %s: %s\n", outfn, libraw_strerror(ret));
  else
    printf("Created %s\n", outfn);
}