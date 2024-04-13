int main(int ac, char *av[])
{
  int i, ret, output_thumbs = 0;

  // don't use fixed size buffers in real apps!

  LibRaw RawProcessor;

  if (ac < 2)
  {
    printf("mem_image - LibRaw sample, to illustrate work for memory buffers. "
           "Emulates dcraw [-4] [-1] [-e] [-h]\n"
           "Usage: %s [-D] [-T] [-v] [-e] raw-files....\n"
           "\t-6 - output 16-bit PPM\n"
           "\t-4 - linear 16-bit data\n"
           "\t-e - extract thumbnails (same as dcraw -e in separate run)\n",
           "\t-h - use half_size\n");
    return 0;
  }

  putenv((char *)"TZ=UTC"); // dcraw compatibility, affects TIFF datestamp field

#define P1 RawProcessor.imgdata.idata
#define S RawProcessor.imgdata.sizes
#define C RawProcessor.imgdata.color
#define T RawProcessor.imgdata.thumbnail
#define P2 RawProcessor.imgdata.other
#define OUT RawProcessor.imgdata.params

  for (i = 1; i < ac; i++)
  {
    if (av[i][0] == '-')
    {
      if (av[i][1] == '6' && av[i][2] == 0)
        OUT.output_bps = 16;
      if (av[i][1] == '4' && av[i][2] == 0)
      {
        OUT.output_bps = 16;
        OUT.gamm[0] = OUT.gamm[1] = OUT.no_auto_bright = 1;
      }
      if (av[i][1] == 'e' && av[i][2] == 0)
        output_thumbs++;
      if (av[i][1] == 'h' && av[i][2] == 0)
        OUT.half_size = 1;
      continue;
    }
    printf("Processing %s\n", av[i]);
    if ((ret = RawProcessor.open_file(av[i])) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot open %s: %s\n", av[i], libraw_strerror(ret));
      continue; // no recycle b/c open file will recycle itself
    }

    if ((ret = RawProcessor.unpack()) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot unpack %s: %s\n", av[i], libraw_strerror(ret));
      continue;
    }

    // we should call dcraw_process before thumbnail extraction because for
    // some cameras (i.e. Kodak ones) white balance for thumbnal should be set
    // from main image settings

    ret = RawProcessor.dcraw_process();

    if (LIBRAW_SUCCESS != ret)
    {
      fprintf(stderr, "Cannot do postpocessing on %s: %s\n", av[i],
              libraw_strerror(ret));
      if (LIBRAW_FATAL_ERROR(ret))
        continue;
    }
    libraw_processed_image_t *image = RawProcessor.dcraw_make_mem_image(&ret);
    if (image)
    {
      write_ppm(image, av[i]);
      LibRaw::dcraw_clear_mem(image);
    }
    else
      fprintf(stderr, "Cannot unpack %s to memory buffer: %s\n", av[i],
              libraw_strerror(ret));

    if (output_thumbs)
    {

      if ((ret = RawProcessor.unpack_thumb()) != LIBRAW_SUCCESS)
      {
        fprintf(stderr, "Cannot unpack_thumb %s: %s\n", av[i],
                libraw_strerror(ret));
        if (LIBRAW_FATAL_ERROR(ret))
          continue; // skip to next file
      }
      else
      {
        libraw_processed_image_t *thumb =
            RawProcessor.dcraw_make_mem_thumb(&ret);
        if (thumb)
        {
          write_thumb(thumb, av[i]);
          LibRaw::dcraw_clear_mem(thumb);
        }
        else
          fprintf(stderr,
                  "Cannot unpack thumbnail of %s to memory buffer: %s\n", av[i],
                  libraw_strerror(ret));
      }
    }

    RawProcessor.recycle(); // just for show this call
  }
  return 0;
}