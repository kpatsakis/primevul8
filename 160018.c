void LibRaw::apply_tiff()
{
  int max_samp = 0, ties = 0, raw = -1, thm = -1, i;
  unsigned long long ns, os;
  struct jhead jh;

  thumb_misc = 16;
  if (thumb_offset)
  {
    fseek(ifp, thumb_offset, SEEK_SET);
    if (ljpeg_start(&jh, 1))
    {
      if ((unsigned)jh.bits < 17 && (unsigned)jh.wide < 0x10000 &&
          (unsigned)jh.high < 0x10000)
      {
        thumb_misc = jh.bits;
        thumb_width = jh.wide;
        thumb_height = jh.high;
      }
    }
  }
  for (i = tiff_nifds; i--;)
  {
    if (tiff_ifd[i].t_shutter)
      shutter = tiff_ifd[i].t_shutter;
    tiff_ifd[i].t_shutter = shutter;
  }

  if(dng_version)
  {
      int ifdc = 0;
      for (i = 0; i < tiff_nifds; i++)
      {
          if (tiff_ifd[i].t_width < 1 || tiff_ifd[i].t_width > 65535 ||
              tiff_ifd[i].t_height < 1 || tiff_ifd[i].t_height > 65535)
              continue; /* wrong image dimensions */

          int samp = tiff_ifd[i].samples;
          if(samp == 2) samp = 1; // Fuji 2-frame
          max_samp = LIM(MAX(max_samp,samp),1,3); // max_samp is needed for thumbnail selection below

          if(tiff_ifd[i].phint != 32803 && tiff_ifd[i].phint != 34892) continue;

          if(
              (tiff_ifd[i].newsubfiletype == 0) // main image
              // Enhanced demosaiced:
              || (tiff_ifd[i].newsubfiletype == 16 && (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_DNG_ADD_ENHANCED))
              // Preview: 0x1 or 0x10001
              || ((tiff_ifd[i].newsubfiletype & 0xffff) == 1 && (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_DNG_ADD_PREVIEWS))
             )
          {
              // Add this IFD to dng_frames
              libraw_internal_data.unpacker_data.dng_frames[ifdc] = ((tiff_ifd[i].newsubfiletype & 0xffff)<<16) | ((i<<8)&0xff00);
              ifdc++;
              // Fuji SuperCCD: second frame:
              if((tiff_ifd[i].newsubfiletype == 0) && tiff_ifd[i].samples == 2)
              {
                  libraw_internal_data.unpacker_data.dng_frames[ifdc] = ((tiff_ifd[i].newsubfiletype & 0xffff)<<16) | ((i<<8)&0xff00) | 1;
                  ifdc++;
              }
          }
      }
      if(ifdc)
      {
          if(ifdc>1 && (imgdata.params.raw_processing_options & LIBRAW_PROCESSING_DNG_PREFER_LARGEST_IMAGE))
          {
              ifd_size_t arr[LIBRAW_IFD_MAXCOUNT*2];
              memset(arr,0,sizeof(arr));
              for(int i = 0; i < ifdc && i < LIBRAW_IFD_MAXCOUNT*2; i++)
              {
                  int ifdidx = (libraw_internal_data.unpacker_data.dng_frames[i]>>8) & 0xff;
                  arr[i].ifdi = libraw_internal_data.unpacker_data.dng_frames[i];
                  arr[i].databits = tiff_ifd[ifdidx].t_width * tiff_ifd[ifdidx].t_height * tiff_ifd[ifdidx].samples * tiff_ifd[ifdidx].bps
                      + (0x100 - (arr[i].ifdi & 0xff)); // add inverted frame # to ensure same sort order for similar sized frames.
              }
              qsort(arr, MIN(ifdc,LIBRAW_IFD_MAXCOUNT*2) ,sizeof(arr[0]),ifd_size_t_cmp);
              for(int i = 0; i < ifdc && i < LIBRAW_IFD_MAXCOUNT*2; i++)
                  libraw_internal_data.unpacker_data.dng_frames[i] = arr[i].ifdi;
          }

          int idx = LIM(shot_select,0,ifdc-1);
          i = (libraw_internal_data.unpacker_data.dng_frames[idx]>>8) & 0xff; // extract frame# back

          raw_width = tiff_ifd[i].t_width;
          raw_height = tiff_ifd[i].t_height;
          tiff_bps = tiff_ifd[i].bps;
          tiff_compress = tiff_ifd[i].comp;
          tiff_sampleformat = tiff_ifd[i].sample_format;
          data_offset = tiff_ifd[i].offset;
          data_size = tiff_ifd[i].bytes;
          tiff_flip = tiff_ifd[i].t_flip;
          tiff_samples = tiff_ifd[i].samples;
          tile_width = tiff_ifd[i].t_tile_width;
          tile_length = tiff_ifd[i].t_tile_length;
          fuji_width = tiff_ifd[i].t_fuji_width;
          if(tiff_samples != 2) /* special case: Fuji SuperCCD */
          {
              if(tiff_ifd[i].phint == 34892)
                  filters = 0;
              else if(i > 0 && tiff_ifd[i].phint == 32803 && tiff_ifd[0].phint == 32803 && !tiff_ifd[i].t_filters && tiff_ifd[0].t_filters)
                  filters = tiff_ifd[0].t_filters;
              else
                  filters = tiff_ifd[i].t_filters;
              width = tiff_ifd[i].t_vwidth;
              height = tiff_ifd[i].t_vheight;
              top_margin = tiff_ifd[i].t_tm;
              left_margin = tiff_ifd[i].t_lm;
              shutter = tiff_ifd[i].t_shutter;
              if(tiff_ifd[i].dng_levels.dng_whitelevel[0])
                  maximum = tiff_ifd[i].dng_levels.dng_whitelevel[0];
              else if (tiff_ifd[i].sample_format <= 2  && tiff_bps > 0 && tiff_bps < 32) // SampleFormat: 0-default(1), 1 - Uint, 2 - Int
                  maximum = (1<<tiff_bps)-1;
              else if (tiff_ifd[i].sample_format == 3)
                  maximum = 1; // Defaults for FP
          }
          raw = i;
          is_raw = ifdc;
      }
      else
          is_raw = 0;
  }
  else
  {
      for (i = 0; i < tiff_nifds; i++)
      {
          if (tiff_ifd[i].t_width < 1 || tiff_ifd[i].t_width > 65535 ||
              tiff_ifd[i].t_height < 1 || tiff_ifd[i].t_height > 65535)
              continue; /* wrong image dimensions */
          if (max_samp < tiff_ifd[i].samples)
              max_samp = tiff_ifd[i].samples;
          if (max_samp > 3)
              max_samp = 3;

          os = raw_width * raw_height;
          ns = tiff_ifd[i].t_width * tiff_ifd[i].t_height;
          if (tiff_bps)
          {
              os *= tiff_bps;
              ns *= tiff_ifd[i].bps;
          }
          if ((tiff_ifd[i].comp != 6 || tiff_ifd[i].samples != 3) &&
              unsigned(tiff_ifd[i].t_width | tiff_ifd[i].t_height) < 0x10000 &&
              (unsigned)tiff_ifd[i].bps < 33 && (unsigned)tiff_ifd[i].samples < 13 &&
              ns && ((ns > os && (ties = 1)) || (ns == os && shot_select == ties++)))
          {
              raw_width = tiff_ifd[i].t_width;
              raw_height = tiff_ifd[i].t_height;
              tiff_bps = tiff_ifd[i].bps;
              tiff_compress = tiff_ifd[i].comp;
              tiff_sampleformat = tiff_ifd[i].sample_format;
              data_offset = tiff_ifd[i].offset;
              data_size = tiff_ifd[i].bytes;
              tiff_flip = tiff_ifd[i].t_flip;
              tiff_samples = tiff_ifd[i].samples;
              tile_width = tiff_ifd[i].t_tile_width;
              tile_length = tiff_ifd[i].t_tile_length;
              shutter = tiff_ifd[i].t_shutter;
              raw = i;
          }
      }
      if (is_raw == 1 && ties)
          is_raw = ties;
  }
  if (is_NikonTransfer && raw >= 0)
  {
    if (tiff_ifd[raw].bps == 16)
    {
      if (tiff_compress == 1)
      {
        if ((raw_width * raw_height * 3) == (tiff_ifd[raw].bytes << 1))
        {
          tiff_bps = tiff_ifd[raw].bps = 12;
        }
        else
        {
          tiff_bps = tiff_ifd[raw].bps = 14;
        }
      }
    }
    else if (tiff_ifd[raw].bps == 8)
    {
      if (tiff_compress == 1)
      {
        is_NikonTransfer = 2; // 8-bit debayered TIFF, like CoolScan NEFs
        imgdata.params.coolscan_nef_gamma = 2.2f;
      }
    }
  }

  if (!tile_width)
    tile_width = INT_MAX;
  if (!tile_length)
    tile_length = INT_MAX;
  for (i = tiff_nifds; i--;)
    if (tiff_ifd[i].t_flip)
      tiff_flip = tiff_ifd[i].t_flip;
  if (raw >= 0 && !load_raw)
    switch (tiff_compress)
    {
    case 32767:
      if (!dng_version &&
          INT64(tiff_ifd[raw].bytes) == INT64(raw_width) * INT64(raw_height))
      {
        tiff_bps = 14;
        load_raw = &LibRaw::sony_arw2_load_raw;
        break;
      }
      if (!dng_version && !strncasecmp(make, "Sony", 4) &&
          INT64(tiff_ifd[raw].bytes) ==
              INT64(raw_width) * INT64(raw_height) * 2ULL)
      {
        tiff_bps = 14;
        load_raw = &LibRaw::unpacked_load_raw;
        break;
      }
      if (INT64(tiff_ifd[raw].bytes) * 8ULL !=
          INT64(raw_width) * INT64(raw_height) * INT64(tiff_bps))
      {
        raw_height += 8;
        load_raw = &LibRaw::sony_arw_load_raw;
        break;
      }
      load_flags = 79;
    case 32769:
      load_flags++;
    case 32770:
    case 32773:
      goto slr;
    case 0:
    case 1:
#ifdef USE_DNGSDK
      if (dng_version && tiff_sampleformat == 3 &&
          (tiff_bps > 8 && (tiff_bps % 8 == 0))) // 16,24,32,48...
      {
        load_raw = &LibRaw::float_dng_load_raw_placeholder;
        break;
      }
#endif
      // Sony 14-bit uncompressed
      if (!dng_version && !strncasecmp(make, "Sony", 4) &&
          INT64(tiff_ifd[raw].bytes) ==
              INT64(raw_width) * INT64(raw_height) * 2ULL)
      {
        tiff_bps = 14;
        load_raw = &LibRaw::unpacked_load_raw;
        break;
      }
      if (!dng_version && !strncasecmp(make, "Sony", 4) &&
          tiff_ifd[raw].samples == 4 &&
          INT64(tiff_ifd[raw].bytes) ==
              INT64(raw_width) * INT64(raw_height) * 8ULL) // Sony ARQ
      {
        // maybe to detect ARQ with the following:
        // if (tiff_ifd[raw].phint == 32892)
        tiff_bps = 14;
        tiff_samples = 4;
        load_raw = &LibRaw::sony_arq_load_raw;
        filters = 0;
        strcpy(cdesc, "RGBG");
        break;
      }
      if (!strncasecmp(make, "Nikon", 5) &&
          (!strncmp(software, "Nikon Scan", 10) || (is_NikonTransfer == 2) || strcasestr(model,"COOLSCAN")))
      {
        load_raw = &LibRaw::nikon_coolscan_load_raw;
        raw_color = 1;
        filters = 0;
        break;
      }
      if ((!strncmp(make, "OLYMPUS", 7) ||
           (!strncasecmp(make, "CLAUSS", 6) &&
            !strncasecmp(model, "piX 5oo", 7))) && // 0x5330303539 works here
          (INT64(tiff_ifd[raw].bytes) * 2ULL ==
           INT64(raw_width) * INT64(raw_height) * 3ULL))
        load_flags = 24;
      if (!dng_version && INT64(tiff_ifd[raw].bytes) * 5ULL ==
                              INT64(raw_width) * INT64(raw_height) * 8ULL)
      {
        load_flags = 81;
        tiff_bps = 12;
      }
    slr:
      switch (tiff_bps)
      {
      case 8:
        load_raw = &LibRaw::eight_bit_load_raw;
        break;
      case 12:
        if (tiff_ifd[raw].phint == 2)
          load_flags = 6;
        if (!strncasecmp(make, "NIKON", 5) &&
            !strncasecmp(model, "COOLPIX A1000", 13) &&
            data_size == raw_width * raw_height * 2)
          load_raw = &LibRaw::unpacked_load_raw;
        else
          load_raw = &LibRaw::packed_load_raw;
        break;
      case 14:
        load_flags = 0;
      case 16:
        load_raw = &LibRaw::unpacked_load_raw;
        if ((!strncmp(make, "OLYMPUS", 7) ||
             (!strncasecmp(make, "CLAUSS", 6) &&
              !strncasecmp(model, "piX 5oo", 7))) && // 0x5330303539 works here
            (INT64(tiff_ifd[raw].bytes) * 7ULL >
             INT64(raw_width) * INT64(raw_height)))
          load_raw = &LibRaw::olympus_load_raw;
      }
      break;
    case 6:
    case 7:
    case 99:
      load_raw = &LibRaw::lossless_jpeg_load_raw;
      break;
    case 262:
      load_raw = &LibRaw::kodak_262_load_raw;
      break;
    case 34713:
      if ((INT64(raw_width) + 9ULL) / 10ULL * 16ULL * INT64(raw_height) ==
          INT64(tiff_ifd[raw].bytes))
      {
        load_raw = &LibRaw::packed_load_raw;
        load_flags = 1;
      }
      else if (INT64(raw_width) * INT64(raw_height) * 3ULL ==
               INT64(tiff_ifd[raw].bytes) * 2ULL)
      {
        load_raw = &LibRaw::packed_load_raw;
        if (model[0] == 'N')
          load_flags = 80;
      }
      else if (INT64(raw_width) * INT64(raw_height) * 3ULL ==
               INT64(tiff_ifd[raw].bytes))
      {
        load_raw = &LibRaw::nikon_yuv_load_raw;
        gamma_curve(1 / 2.4, 12.92, 1, 4095);
        memset(cblack, 0, sizeof cblack);
        filters = 0;
      }
      else if (INT64(raw_width) * INT64(raw_height) * 2ULL ==
               INT64(tiff_ifd[raw].bytes))
      {
        load_raw = &LibRaw::unpacked_load_raw;
        load_flags = 4;
        order = 0x4d4d;
      }
      else if (INT64(raw_width) * INT64(raw_height) * 3ULL ==
               INT64(tiff_ifd[raw].bytes) * 2ULL)
      {
        load_raw = &LibRaw::packed_load_raw;
        load_flags = 80;
      }
      else if (tiff_ifd[raw].rows_per_strip &&
               tiff_ifd[raw].strip_offsets_count &&
               tiff_ifd[raw].strip_offsets_count ==
                   tiff_ifd[raw].strip_byte_counts_count)
      {
        int fit = 1;
        for (int i = 0; i < tiff_ifd[raw].strip_byte_counts_count - 1;
             i++) // all but last
          if (INT64(tiff_ifd[raw].strip_byte_counts[i]) * 2ULL !=
              INT64(tiff_ifd[raw].rows_per_strip) * INT64(raw_width) * 3ULL)
          {
            fit = 0;
            break;
          }
        if (fit)
          load_raw = &LibRaw::nikon_load_striped_packed_raw;
        else
          load_raw = &LibRaw::nikon_load_raw; // fallback
      }
      else if ((((INT64(raw_width) * 3ULL / 2ULL) + 15ULL) / 16ULL) * 16ULL *
                   INT64(raw_height) ==
               INT64(tiff_ifd[raw].bytes))
      {
        load_raw = &LibRaw::nikon_load_padded_packed_raw;
        load_flags = (((INT64(raw_width) * 3ULL / 2ULL) + 15ULL) / 16ULL) *
                     16ULL; // bytes per row
      }
      else
        load_raw = &LibRaw::nikon_load_raw;
      break;
    case 65535:
      load_raw = &LibRaw::pentax_load_raw;
      break;
    case 65000:
      switch (tiff_ifd[raw].phint)
      {
      case 2:
        load_raw = &LibRaw::kodak_rgb_load_raw;
        filters = 0;
        break;
      case 6:
        load_raw = &LibRaw::kodak_ycbcr_load_raw;
        filters = 0;
        break;
      case 32803:
        load_raw = &LibRaw::kodak_65000_load_raw;
      }
    case 32867:
    case 34892:
      break;
    case 8:
      break;
#ifdef USE_GPRSDK
    case 9:
        if(dng_version)
            break; /* Compression=9 supported for dng if we compiled with GPR SDK */
        /* Else: fallthrough */
#endif
    default:
      is_raw = 0;
    }
  if (!dng_version)
    if (((tiff_samples == 3 && tiff_ifd[raw].bytes &&
          !(tiff_bps == 16 &&
            !strncmp(make, "Leaf", 4)) && // Allow Leaf/16bit/3color files
          tiff_bps != 14 &&
          (tiff_compress & -16) != 32768) ||
         (tiff_bps == 8 && strncmp(make, "Phase", 5) &&
          strncmp(make, "Leaf", 4) && !strcasestr(make, "Kodak") &&
          !strstr(model2, "DEBUG RAW"))) && !strcasestr(model,"COOLSCAN") &&
        strncmp(software, "Nikon Scan", 10) && is_NikonTransfer != 2)
      is_raw = 0;

  for (i = 0; i < tiff_nifds; i++)
    if (i != raw &&
        (tiff_ifd[i].samples == max_samp ||
         (tiff_ifd[i].comp == 7 &&
          tiff_ifd[i].samples == 1)) /* Allow 1-bps JPEGs */
        && tiff_ifd[i].bps > 0 && tiff_ifd[i].bps < 33 &&
        tiff_ifd[i].phint != 32803 && tiff_ifd[i].phint != 34892 &&
        unsigned(tiff_ifd[i].t_width | tiff_ifd[i].t_height) < 0x10000 &&
        tiff_ifd[i].t_width * tiff_ifd[i].t_height /
                (SQR(tiff_ifd[i].bps) + 1) >
            thumb_width * thumb_height / (SQR(thumb_misc) + 1) &&
        tiff_ifd[i].comp != 34892)
    {
      thumb_width = tiff_ifd[i].t_width;
      thumb_height = tiff_ifd[i].t_height;
      thumb_offset = tiff_ifd[i].offset;
      thumb_length = tiff_ifd[i].bytes;
      thumb_misc = tiff_ifd[i].bps;
      thm = i;
    }
  if (thm >= 0)
  {
    thumb_misc |= tiff_ifd[thm].samples << 5;
    switch (tiff_ifd[thm].comp)
    {
    case 0:
      write_thumb = &LibRaw::layer_thumb;
      break;
    case 1:
      if (tiff_ifd[thm].bps <= 8)
        write_thumb = &LibRaw::ppm_thumb;
      else if (!strncmp(make, "Imacon", 6))
        write_thumb = &LibRaw::ppm16_thumb;
      else
        thumb_load_raw = &LibRaw::kodak_thumb_load_raw;
      break;
    case 65000:
      thumb_load_raw = tiff_ifd[thm].phint == 6 ? &LibRaw::kodak_ycbcr_load_raw
                                                : &LibRaw::kodak_rgb_load_raw;
    }
  }
}