int main(int ac, char *av[])
{
  int i, ret;
  int verbose = 1, autoscale = 0, use_gamma = 0, out_tiff = 0;
  char outfn[1024];

  LibRaw RawProcessor;
  if (ac < 2)
  {
  usage:
    printf("unprocessed_raw - LibRaw %s sample. %d cameras supported\n"
           "Usage: %s [-q] [-A] [-g] [-s N] raw-files....\n"
           "\t-q - be quiet\n"
           "\t-s N - select Nth image in file (default=0)\n"
           "\t-g - use gamma correction with gamma 2.2 (not precise,use for "
           "visual inspection only)\n"
           "\t-A - autoscaling (by integer factor)\n"
           "\t-T - write tiff instead of pgm\n",
           LibRaw::version(), LibRaw::cameraCount(), av[0]);
    return 0;
  }

#define S RawProcessor.imgdata.sizes
#define OUT RawProcessor.imgdata.params

  for (i = 1; i < ac; i++)
  {
    if (av[i][0] == '-')
    {
      if (av[i][1] == 'q' && av[i][2] == 0)
        verbose = 0;
      else if (av[i][1] == 'A' && av[i][2] == 0)
        autoscale = 1;
      else if (av[i][1] == 'g' && av[i][2] == 0)
        use_gamma = 1;
      else if (av[i][1] == 'T' && av[i][2] == 0)
        out_tiff = 1;
      else if (av[i][1] == 's' && av[i][2] == 0)
      {
        i++;
        OUT.shot_select = av[i] ? atoi(av[i]) : 0;
      }
      else
        goto usage;
      continue;
    }

    if (verbose)
      printf("Processing file %s\n", av[i]);
    if ((ret = RawProcessor.open_file(av[i])) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot open %s: %s\n", av[i], libraw_strerror(ret));
      continue; // no recycle b/c open file will recycle itself
    }
    if (verbose)
    {
      printf("Image size: %dx%d\nRaw size: %dx%d\n", S.width, S.height,
             S.raw_width, S.raw_height);
      printf("Margins: top=%d, left=%d\n", S.top_margin, S.left_margin);
    }

    if ((ret = RawProcessor.unpack()) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot unpack %s: %s\n", av[i], libraw_strerror(ret));
      continue;
    }

    if (verbose)
      printf("Unpacked....\n");

    if (!(RawProcessor.imgdata.idata.filters ||
          RawProcessor.imgdata.idata.colors == 1))
    {
      printf("Only Bayer-pattern RAW files supported, sorry....\n");
      continue;
    }

    if (autoscale)
    {
      unsigned max = 0, scale;
      for (int j = 0; j < S.raw_height * S.raw_width; j++)
        if (max < RawProcessor.imgdata.rawdata.raw_image[j])
          max = RawProcessor.imgdata.rawdata.raw_image[j];
      if (max > 0 && max < 1 << 15)
      {
        scale = (1 << 16) / max;
        if (verbose)
          printf("Scaling with multiplier=%d (max=%d)\n", scale, max);

        for (int j = 0; j < S.raw_height * S.raw_width; j++)
          RawProcessor.imgdata.rawdata.raw_image[j] *= scale;
      }
    }
    if (use_gamma)
    {
      unsigned short curve[0x10000];
      gamma_curve(curve);
      for (int j = 0; j < S.raw_height * S.raw_width; j++)
        RawProcessor.imgdata.rawdata.raw_image[j] =
            curve[RawProcessor.imgdata.rawdata.raw_image[j]];
      if (verbose)
        printf("Gamma-corrected....\n");
    }

    if (OUT.shot_select)
      snprintf(outfn, sizeof(outfn), "%s-%d.%s", av[i], OUT.shot_select,
               out_tiff ? "tiff" : "pgm");
    else
      snprintf(outfn, sizeof(outfn), "%s.%s", av[i], out_tiff ? "tiff" : "pgm");

    if (out_tiff)
      write_tiff(S.raw_width, S.raw_height,
                 RawProcessor.imgdata.rawdata.raw_image, outfn);
    else
      write_ppm(S.raw_width, S.raw_height,
                RawProcessor.imgdata.rawdata.raw_image, outfn);

    if (verbose)
      printf("Stored to file %s\n", outfn);
  }
  return 0;
}