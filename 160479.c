int main(int argc, char *argv[])
{
  if (argc == 1)
    usage(argv[0]);

  LibRaw RawProcessor;
  int i, arg, c, ret;
  char opm, opt, *cp, *sp;
  int use_bigfile = 0, use_timing = 0, use_mem = 0, use_mmap = 0;
  char *outext = NULL;
#ifdef USE_DNGSDK
  dng_host *dnghost = NULL;
#endif
  struct file_mapping mapping;
  void *iobuffer = 0;
#ifdef OUT
#undef OUT
#endif
#define OUT RawProcessor.imgdata.params

  argv[argc] = (char *)"";
  for (arg = 1; (((opm = argv[arg][0]) - 2) | 2) == '+';)
  {
    char *optstr = argv[arg];
    opt = argv[arg++][1];
    if ((cp = strchr(sp = (char *)"cnbrkStqmHABCgU", opt)) != 0)
      for (i = 0; i < "111411111144221"[cp - sp] - '0'; i++)
        if (!isdigit(argv[arg + i][0]) && !optstr[2])
        {
          fprintf(stderr, "Non-numeric argument to \"-%c\"\n", opt);
          return 1;
        }
    if (!strchr("ftdeam", opt) && argv[arg - 1][2])
      fprintf(stderr, "Unknown option \"%s\".\n", argv[arg - 1]);
    switch (opt)
    {
    case 'v':
      verbosity++;
      break;
    case 'G':
      OUT.green_matching = 1;
      break;
    case 'c':
      OUT.adjust_maximum_thr = (float)atof(argv[arg++]);
      break;
    case 'U':
      OUT.auto_bright_thr = (float)atof(argv[arg++]);
      break;
    case 'n':
      OUT.threshold = (float)atof(argv[arg++]);
      break;
    case 'b':
      OUT.bright = (float)atof(argv[arg++]);
      break;
    case 'P':
      OUT.bad_pixels = argv[arg++];
      break;
    case 'K':
      OUT.dark_frame = argv[arg++];
      break;
    case 'r':
      for (c = 0; c < 4; c++)
        OUT.user_mul[c] = (float)atof(argv[arg++]);
      break;
    case 'C':
      OUT.aber[0] = 1 / atof(argv[arg++]);
      OUT.aber[2] = 1 / atof(argv[arg++]);
      break;
    case 'g':
      OUT.gamm[0] = 1 / atof(argv[arg++]);
      OUT.gamm[1] = atof(argv[arg++]);
      break;
    case 'k':
      OUT.user_black = atoi(argv[arg++]);
      break;
    case 'S':
      OUT.user_sat = atoi(argv[arg++]);
      break;
    case 'R':
      OUT.raw_processing_options = atoi(argv[arg++]);
      break;
    case 't':
      if (!strcmp(optstr, "-timing"))
        use_timing = 1;
      else if (!argv[arg - 1][2])
        OUT.user_flip = atoi(argv[arg++]);
      else
        fprintf(stderr, "Unknown option \"%s\".\n", argv[arg - 1]);
      break;
    case 'q':
      OUT.user_qual = atoi(argv[arg++]);
      break;
    case 'm':
      if (!strcmp(optstr, "-mmap"))
        use_mmap = 1;
      else
          if (!strcmp(optstr, "-mem"))
        use_mem = 1;
      else
      {
        if (!argv[arg - 1][2])
          OUT.med_passes = atoi(argv[arg++]);
        else
          fprintf(stderr, "Unknown option \"%s\".\n", argv[arg - 1]);
      }
      break;
    case 'H':
      OUT.highlight = atoi(argv[arg++]);
      break;
    case 's':
      OUT.shot_select = abs(atoi(argv[arg++]));
      break;
    case 'o':
      if (isdigit(argv[arg][0]) && !isdigit(argv[arg][1]))
        OUT.output_color = atoi(argv[arg++]);
#ifndef NO_LCMS
      else
        OUT.output_profile = argv[arg++];
      break;
    case 'p':
      OUT.camera_profile = argv[arg++];
#endif
      break;
    case 'h':
      OUT.half_size = 1;
      break;
    case 'f':
      if (!strcmp(optstr, "-fbdd"))
        OUT.fbdd_noiserd = atoi(argv[arg++]);
      else
      {
        if (!argv[arg - 1][2])
          OUT.four_color_rgb = 1;
        else
          fprintf(stderr, "Unknown option \"%s\".\n", argv[arg - 1]);
      }
      break;
    case 'A':
      for (c = 0; c < 4; c++)
        OUT.greybox[c] = atoi(argv[arg++]);
      break;
    case 'B':
      for (c = 0; c < 4; c++)
        OUT.cropbox[c] = atoi(argv[arg++]);
      break;
    case 'a':
      if (!strcmp(optstr, "-aexpo"))
      {
        OUT.exp_correc = 1;
        OUT.exp_shift = (float)atof(argv[arg++]);
        OUT.exp_preser = (float)atof(argv[arg++]);
      }
      else if (!strcmp(optstr, "-apentax4shot"))
      {
        OUT.raw_processing_options |= LIBRAW_PROCESSING_PENTAX_PS_ALLFRAMES;
      }
      else if (!strcmp(optstr, "-apentax4shotorder"))
      {
        strncpy(OUT.p4shot_order, argv[arg++], 5);
      }
      else if (!argv[arg - 1][2])
        OUT.use_auto_wb = 1;
      else
        fprintf(stderr, "Unknown option \"%s\".\n", argv[arg - 1]);
      break;
    case 'w':
      OUT.use_camera_wb = 1;
      break;
    case 'M':
      OUT.use_camera_matrix = (opm == '+')?3:0;
      break;
    case 'j':
      OUT.use_fuji_rotate = 0;
      break;
    case 'W':
      OUT.no_auto_bright = 1;
      break;
    case 'T':
      OUT.output_tiff = 1;
      break;
    case '4':
      OUT.gamm[0] = OUT.gamm[1] = OUT.no_auto_bright = 1; /* no break here! */
    case '6':
      OUT.output_bps = 16;
      break;
    case 'F':
      use_bigfile = 1;
      break;
    case 'Z':
      outext = strdup(argv[arg++]);
      break;
    case 'd':
      if (!strcmp(optstr, "-dcbi"))
        OUT.dcb_iterations = atoi(argv[arg++]);
      else if (!strcmp(optstr, "-disars"))
        OUT.use_rawspeed = 0;
      else if (!strcmp(optstr, "-disinterp"))
        OUT.no_interpolation = 1;
      else if (!strcmp(optstr, "-dcbe"))
        OUT.dcb_enhance_fl = 1;
      else if (!strcmp(optstr, "-dsrawrgb1"))
      {
        OUT.raw_processing_options |= LIBRAW_PROCESSING_SRAW_NO_RGB;
        OUT.raw_processing_options &= ~LIBRAW_PROCESSING_SRAW_NO_INTERPOLATE;
      }
      else if (!strcmp(optstr, "-dsrawrgb2"))
      {
        OUT.raw_processing_options &= ~LIBRAW_PROCESSING_SRAW_NO_RGB;
        OUT.raw_processing_options |= LIBRAW_PROCESSING_SRAW_NO_INTERPOLATE;
      }
#ifdef USE_DNGSDK
      else if (!strcmp(optstr, "-dngsdk"))
      {
        dnghost = new dng_host;
        RawProcessor.set_dng_host(dnghost);
      }
      else if (!strcmp(optstr, "-dngflags"))
      {
        OUT.use_dngsdk = atoi(argv[arg++]);
      }
#endif
      else
        fprintf(stderr, "Unknown option \"%s\".\n", argv[arg - 1]);
      break;
    default:
      fprintf(stderr, "Unknown option \"-%c\".\n", opt);
      return 1;
    }
  }
#ifndef LIBRAW_WIN32_CALLS
  putenv((char *)"TZ=UTC"); // dcraw compatibility, affects TIFF datestamp field
#else
  _putenv(
      (char *)"TZ=UTC"); // dcraw compatibility, affects TIFF datestamp field
#endif
#define P1 RawProcessor.imgdata.idata
#define S RawProcessor.imgdata.sizes
#define C RawProcessor.imgdata.color
#define T RawProcessor.imgdata.thumbnail
#define P2 RawProcessor.imgdata.other

  if (outext && !strcmp(outext, "-"))
    use_timing = verbosity = 0;

  if (verbosity > 1)
    RawProcessor.set_progress_handler(my_progress_callback,
                                      (void *)"Sample data passed");
#ifdef LIBRAW_USE_OPENMP
  if (verbosity)
    printf("Using %d threads\n", omp_get_max_threads());
#endif

  for (; arg < argc; arg++)
  {
    char outfn[1024];

    if (verbosity)
      printf("Processing file %s\n", argv[arg]);

    timerstart();

	if (use_mmap)
	{
		create_mapping(mapping, argv[arg]);
		if (!mapping.map)
		{
			fprintf(stderr, "Cannot map %s\n", argv[arg]);
			close_mapping(mapping);
			continue;
		}
      if ((ret = RawProcessor.open_buffer(mapping.map,mapping.fsize) !=
                 LIBRAW_SUCCESS))
      {
        fprintf(stderr, "Cannot open_buffer %s: %s\n", argv[arg], libraw_strerror(ret));
		close_mapping(mapping);
        continue; // no recycle b/c open file will recycle itself
      }
    }
    else  if (use_mem)
    {
      int file = open(argv[arg], O_RDONLY | O_BINARY);
      struct stat st;
      if (file < 0)
      {
        fprintf(stderr, "Cannot open %s: %s\n", argv[arg], strerror(errno));
        continue;
      }
      if (fstat(file, &st))
      {
        fprintf(stderr, "Cannot stat %s: %s\n", argv[arg], strerror(errno));
        close(file);
        continue;
      }
      if (!(iobuffer = malloc(st.st_size)))
      {
        fprintf(stderr, "Cannot allocate %d kbytes for memory buffer\n",
                (int)(st.st_size / 1024));
        close(file);
        continue;
      }
      int rd;
      if (st.st_size != (rd = read(file, iobuffer, st.st_size)))
      {
        fprintf(stderr,
                "Cannot read %d bytes instead of  %d to memory buffer\n",
                (int)rd, (int)st.st_size);
        close(file);
        free(iobuffer);
        continue;
      }
      close(file);
      if ((ret = RawProcessor.open_buffer(iobuffer, st.st_size)) !=
          LIBRAW_SUCCESS)
      {
        fprintf(stderr, "Cannot open_buffer %s: %s\n", argv[arg],
                libraw_strerror(ret));
        free(iobuffer);
        continue; // no recycle b/c open file will recycle itself
      }
    }
    else
    {
      if (use_bigfile)
        // force open_file switch to bigfile processing
        ret = RawProcessor.open_file(argv[arg], 1);
      else
        ret = RawProcessor.open_file(argv[arg]);

      if (ret != LIBRAW_SUCCESS)
      {
        fprintf(stderr, "Cannot open %s: %s\n", argv[arg],
                libraw_strerror(ret));
        continue; // no recycle b/c open_file will recycle itself
      }
    }

    if (use_timing)
      timerprint("LibRaw::open_file()", argv[arg]);

    timerstart();
    if ((ret = RawProcessor.unpack()) != LIBRAW_SUCCESS)
    {
      fprintf(stderr, "Cannot unpack %s: %s\n", argv[arg],
              libraw_strerror(ret));
      continue;
    }

    if (use_timing)
      timerprint("LibRaw::unpack()", argv[arg]);

    timerstart();
    if (LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_process()))
    {
      fprintf(stderr, "Cannot do postpocessing on %s: %s\n", argv[arg],
              libraw_strerror(ret));
      if (LIBRAW_FATAL_ERROR(ret))
        continue;
    }
    if (use_timing)
      timerprint("LibRaw::dcraw_process()", argv[arg]);

    if (!outext)
      snprintf(outfn, sizeof(outfn), "%s.%s", argv[arg],
               OUT.output_tiff ? "tiff" : (P1.colors > 1 ? "ppm" : "pgm"));
    else if (!strcmp(outext, "-"))
      snprintf(outfn, sizeof(outfn), "-");
    else
    {
      if (*outext == '.') // append
        snprintf(outfn, sizeof(outfn), "%s%s", argv[arg], outext);
      else if (strchr(outext, '.') && *outext != '.') // dot is not 1st char
        strncpy(outfn, outext, sizeof(outfn));
      else
      {
        strncpy(outfn, argv[arg], sizeof(outfn));
        if (strlen(outfn) > 0)
        {
          char *lastchar = outfn + strlen(outfn); // points to term 0
          while (--lastchar > outfn)
          {
            if (*lastchar == '/' || *lastchar == '\\')
              break;
            if (*lastchar == '.')
            {
              *lastchar = 0;
              break;
            };
          }
        }
        strncat(outfn, ".", sizeof(outfn) - strlen(outfn) - 1);
        strncat(outfn, outext, sizeof(outfn) - strlen(outfn) - 1);
      }
    }

    if (verbosity)
    {
      printf("Writing file %s\n", outfn);
    }

    if (LIBRAW_SUCCESS != (ret = RawProcessor.dcraw_ppm_tiff_writer(outfn)))
      fprintf(stderr, "Cannot write %s: %s\n", outfn, libraw_strerror(ret));

	RawProcessor.recycle(); // just for show this call

	if (use_mmap && mapping.map)
		close_mapping(mapping);
    else if (use_mem && iobuffer)
    {
      free(iobuffer);
      iobuffer = 0;
    }
  }
#ifdef USE_DNGSDK
  if (dnghost)
    delete dnghost;
#endif
  return 0;
}