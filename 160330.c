int LibRaw::raw2image(void)
{

  CHECK_ORDER_LOW(LIBRAW_PROGRESS_LOAD_RAW);

  try
  {
    raw2image_start();

    if (is_phaseone_compressed() && imgdata.rawdata.raw_alloc)
    {
      phase_one_allocate_tempbuffer();
      int rc = phase_one_subtract_black((ushort *)imgdata.rawdata.raw_alloc,
                                        imgdata.rawdata.raw_image);
      if (rc == 0)
        rc = phase_one_correct();
      if (rc != 0)
      {
        phase_one_free_tempbuffer();
        return rc;
      }
    }

    // free and re-allocate image bitmap
    if (imgdata.image)
    {
      imgdata.image = (ushort(*)[4])realloc(
          imgdata.image, S.iheight * S.iwidth * sizeof(*imgdata.image));
      memset(imgdata.image, 0, S.iheight * S.iwidth * sizeof(*imgdata.image));
    }
    else
      imgdata.image =
          (ushort(*)[4])calloc(S.iheight * S.iwidth, sizeof(*imgdata.image));

    merror(imgdata.image, "raw2image()");

    libraw_decoder_info_t decoder_info;
    get_decoder_info(&decoder_info);

    // Move saved bitmap to imgdata.image
    if ((imgdata.idata.filters || P1.colors == 1) && imgdata.rawdata.raw_image)
    {
      if (IO.fuji_width)
      {
        unsigned r, c;
        int row, col;
        for (row = 0; row < S.raw_height - S.top_margin * 2; row++)
        {
          for (col = 0;
               col < IO.fuji_width
                         << !libraw_internal_data.unpacker_data.fuji_layout;
               col++)
          {
            if (libraw_internal_data.unpacker_data.fuji_layout)
            {
              r = IO.fuji_width - 1 - col + (row >> 1);
              c = col + ((row + 1) >> 1);
            }
            else
            {
              r = IO.fuji_width - 1 + row - (col >> 1);
              c = row + ((col + 1) >> 1);
            }
            if (r < S.height && c < S.width)
              imgdata.image[((r) >> IO.shrink) * S.iwidth + ((c) >> IO.shrink)]
                           [FC(r, c)] =
                  imgdata.rawdata
                      .raw_image[(row + S.top_margin) * S.raw_pitch / 2 +
                                 (col + S.left_margin)];
          }
        }
      }
      else
      {
        int row, col;
        for (row = 0; row < S.height; row++)
          for (col = 0; col < S.width; col++)
            imgdata.image[((row) >> IO.shrink) * S.iwidth +
                          ((col) >> IO.shrink)][fcol(row, col)] =
                imgdata.rawdata
                    .raw_image[(row + S.top_margin) * S.raw_pitch / 2 +
                               (col + S.left_margin)];
      }
    }
    else // if(decoder_info.decoder_flags & LIBRAW_DECODER_LEGACY)
    {
      if (imgdata.rawdata.color4_image)
      {
        if (S.width * 8 == S.raw_pitch)
          memmove(imgdata.image, imgdata.rawdata.color4_image,
                  S.width * S.height * sizeof(*imgdata.image));
        else
        {
          for (int row = 0; row < S.height; row++)
            memmove(&imgdata.image[row * S.width],
                    &imgdata.rawdata
                         .color4_image[(row + S.top_margin) * S.raw_pitch / 8 +
                                       S.left_margin],
                    S.width * sizeof(*imgdata.image));
        }
      }
      else if (imgdata.rawdata.color3_image)
      {
        unsigned char *c3image = (unsigned char *)imgdata.rawdata.color3_image;
        for (int row = 0; row < S.height; row++)
        {
          ushort(*srcrow)[3] =
              (ushort(*)[3]) & c3image[(row + S.top_margin) * S.raw_pitch];
          ushort(*dstrow)[4] = (ushort(*)[4]) & imgdata.image[row * S.width];
          for (int col = 0; col < S.width; col++)
          {
            for (int c = 0; c < 3; c++)
              dstrow[col][c] = srcrow[S.left_margin + col][c];
            dstrow[col][3] = 0;
          }
        }
      }
      else
      {
        // legacy decoder, but no data?
        throw LIBRAW_EXCEPTION_DECODE_RAW;
      }
    }

    // Free PhaseOne separate copy allocated at function start
    if (is_phaseone_compressed())
    {
      phase_one_free_tempbuffer();
    }
    // hack - clear later flags!

    if (load_raw == &LibRaw::canon_600_load_raw && S.width < S.raw_width)
    {
      canon_600_correct();
    }

    imgdata.progress_flags =
        LIBRAW_PROGRESS_START | LIBRAW_PROGRESS_OPEN |
        LIBRAW_PROGRESS_RAW2_IMAGE | LIBRAW_PROGRESS_IDENTIFY |
        LIBRAW_PROGRESS_SIZE_ADJUST | LIBRAW_PROGRESS_LOAD_RAW;
    return 0;
  }
  catch (LibRaw_exceptions err)
  {
    EXCEPTION_HANDLER(err);
  }
}