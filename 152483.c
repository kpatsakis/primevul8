void CLASS parse_exif (int base)
{
  unsigned kodak, entries, tag, type, len, save, c;
  double expo,ape;

  kodak = !strncmp(make,"EASTMAN",7) && tiff_nifds < 3;
  entries = get2();
  if(!strncmp(make,"Hasselblad",10) && (tiff_nifds > 3) && (entries > 512)) return;
  while (entries--) {
    tiff_get (base, &tag, &type, &len, &save);

#ifdef LIBRAW_LIBRARY_BUILD
    if(callbacks.exif_cb)
      {
        int savepos = ftell(ifp);
        callbacks.exif_cb(callbacks.exifparser_data,tag,type,len,order,ifp);
        fseek(ifp,savepos,SEEK_SET);
      }
#endif
    switch (tag) {
#ifdef LIBRAW_LIBRARY_BUILD
    case 0xa405:		// FocalLengthIn35mmFormat
      imgdata.lens.FocalLengthIn35mmFormat = get2();
      break;
    case 0xa432:		// LensInfo, 42034dec, Lens Specification per EXIF standard
      imgdata.lens.MinFocal = getreal(type);
      imgdata.lens.MaxFocal = getreal(type);
      imgdata.lens.MaxAp4MinFocal = getreal(type);
      imgdata.lens.MaxAp4MaxFocal = getreal(type);
      break;
    case 0xc630:		// DNG LensInfo, Lens Specification per EXIF standard
      imgdata.lens.dng.MinFocal = getreal(type);
      imgdata.lens.dng.MaxFocal = getreal(type);
      imgdata.lens.dng.MaxAp4MinFocal = getreal(type);
      imgdata.lens.dng.MaxAp4MaxFocal = getreal(type);
      break;
    case 0xa433:		// LensMake
      fread(imgdata.lens.LensMake, MIN(len,sizeof(imgdata.lens.LensMake)), 1, ifp);
      break;
    case 0xa434:		// LensModel
      fread(imgdata.lens.Lens, MIN(len, sizeof(imgdata.lens.LensMake)), 1, ifp);
      if (!strncmp(imgdata.lens.Lens, "----", 4))
        imgdata.lens.Lens[0] = 0;
      break;
    case 0x9205:
      imgdata.lens.EXIF_MaxAp = powf64(2.0f, (getreal(type) / 2.0f));
      break;
#endif
      case 33434:  shutter = getreal(type);		break;
      case 33437:  aperture = getreal(type);		break;  // 0x829d FNumber
      case 34855:  iso_speed = get2();			break;
      case 34866:
        if (iso_speed == 0xffff && (!strncasecmp(make, "SONY",4) || !strncasecmp(make, "CANON",5)))
          iso_speed = getreal(type);
        break;
      case 36867:
      case 36868:  get_timestamp(0);			break;
      case 37377:  if ((expo = -getreal(type)) < 128 && shutter == 0.)
          shutter = powf64(2.0, expo);		break;
      case 37378:                                       // 0x9202 ApertureValue
        if ((fabs(ape = getreal(type))<256.0) && (!aperture))
          aperture = powf64(2.0, ape/2);
        break;
      case 37385:  flash_used = getreal(type);          break;
      case 37386:  focal_len = getreal(type);		break;
      case 37500:  parse_makernote (base, 0);		break;	// tag 0x927c
      case 40962:  if (kodak) raw_width  = get4();	break;
      case 40963:  if (kodak) raw_height = get4();	break;
      case 41730:
	if (get4() == 0x20002)
	  for (exif_cfa=c=0; c < 8; c+=2)
	    exif_cfa |= fgetc(ifp) * 0x01010101 << c;
    }
    fseek (ifp, save, SEEK_SET);
  }
}