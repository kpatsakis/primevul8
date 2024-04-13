void CLASS parse_foveon()
{
  int entries, img=0, off, len, tag, save, i, wide, high, pent, poff[256][2];
  char name[64], value[64];
  order = 0x4949;			/* Little-endian */
  fseek (ifp, 36, SEEK_SET);
  flip = get4();
  fseek (ifp, -4, SEEK_END);
  fseek (ifp, get4(), SEEK_SET);
  if (get4() != 0x64434553) return;	/* SECd */
  entries = (get4(),get4());
  while (entries--) {
    off = get4();
    len = get4();
    tag = get4();
    save = ftell(ifp);
    fseek (ifp, off, SEEK_SET);
    if (get4() != (0x20434553 | (tag << 24))) return;
    switch (tag) {
      case 0x47414d49:			/* IMAG */
      case 0x32414d49:			/* IMA2 */
	fseek (ifp, 8, SEEK_CUR);
	pent = get4();
	wide = get4();
	high = get4();
	if (wide > raw_width && high > raw_height) {
	  switch (pent) {
	    case  5:  load_flags = 1;
	    case  6:  load_raw = &CLASS foveon_sd_load_raw;  break;
	    case 30:  load_raw = &CLASS foveon_dp_load_raw;  break;
	    default:  load_raw = 0;
	  }
	  raw_width  = wide;
	  raw_height = high;
	  data_offset = off+28;
	  is_foveon = 1;
	}
	fseek (ifp, off+28, SEEK_SET);
	if (fgetc(ifp) == 0xff && fgetc(ifp) == 0xd8
		&& thumb_length < len-28) {
	  thumb_offset = off+28;
	  thumb_length = len-28;
	  write_thumb = &CLASS jpeg_thumb;
	}
	if (++img == 2 && !thumb_length) {
	  thumb_offset = off+24;
	  thumb_width = wide;
	  thumb_height = high;
	  write_thumb = &CLASS foveon_thumb;
	}
	break;
      case 0x464d4143:			/* CAMF */
	meta_offset = off+8;
	meta_length = len-28;
	break;
      case 0x504f5250:			/* PROP */
	pent = (get4(),get4());
	fseek (ifp, 12, SEEK_CUR);
	off += pent*8 + 24;
	if ((unsigned) pent > 256) pent=256;
	for (i=0; i < pent*2; i++)
	  ((int *)poff)[i] = off + get4()*2;
	for (i=0; i < pent; i++) {
	  foveon_gets (poff[i][0], name, 64);
	  foveon_gets (poff[i][1], value, 64);
	  if (!strcmp (name, "ISO"))
	    iso_speed = atoi(value);
	  if (!strcmp (name, "CAMMANUF"))
	    strcpy (make, value);
	  if (!strcmp (name, "CAMMODEL"))
	    strcpy (model, value);
	  if (!strcmp (name, "WB_DESC"))
	    strcpy (model2, value);
	  if (!strcmp (name, "TIME"))
	    timestamp = atoi(value);
	  if (!strcmp (name, "EXPTIME"))
	    shutter = atoi(value) / 1000000.0;
	  if (!strcmp (name, "APERTURE"))
	    aperture = atof(value);
	  if (!strcmp (name, "FLENGTH"))
	    focal_len = atof(value);
#ifdef LIBRAW_LIBRARY_BUILD
	  if (!strcmp (name, "FLEQ35MM"))
            imgdata.lens.makernotes.FocalLengthIn35mmFormat = atof(value);
          if (!strcmp (name, "LENSARANGE"))
            {
              char *sp;
              imgdata.lens.makernotes.MaxAp4CurFocal = imgdata.lens.makernotes.MinAp4CurFocal = atof(value);
              sp = strrchr (value, ' ');
              if (sp)
                {
                  imgdata.lens.makernotes.MinAp4CurFocal = atof(sp);
                  if (imgdata.lens.makernotes.MaxAp4CurFocal > imgdata.lens.makernotes.MinAp4CurFocal)
                    my_swap (float, imgdata.lens.makernotes.MaxAp4CurFocal, imgdata.lens.makernotes.MinAp4CurFocal);
                }
            }
          if (!strcmp (name, "LENSFRANGE"))
            {
              char *sp;
              imgdata.lens.makernotes.MinFocal = imgdata.lens.makernotes.MaxFocal = atof(value);
              sp = strrchr (value, ' ');
              if (sp)
                {
                  imgdata.lens.makernotes.MaxFocal = atof(sp);
                  if ((imgdata.lens.makernotes.MaxFocal + 0.17f) < imgdata.lens.makernotes.MinFocal)
                    my_swap (float, imgdata.lens.makernotes.MaxFocal, imgdata.lens.makernotes.MinFocal);
                }
            }
          if (!strcmp (name, "LENSMODEL"))
            {
              imgdata.lens.makernotes.LensID = atoi(value);
              if (imgdata.lens.makernotes.LensID)
                imgdata.lens.makernotes.LensMount = Sigma_X3F;
            }
        }
#endif
    }
#ifdef LOCALTIME
	timestamp = mktime (gmtime (&timestamp));
#endif
    }
    fseek (ifp, save, SEEK_SET);
  }