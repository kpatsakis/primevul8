void LibRaw::x3f_load_raw()
{
  // already in try/catch
  int raise_error = 0;
  x3f_t *x3f = (x3f_t *)_x3f_data;
  if (!x3f)
    return; // No data pointer set
  if (X3F_OK == x3f_load_data(x3f, x3f_get_raw(x3f)))
  {
    x3f_directory_entry_t *DE = x3f_get_raw(x3f);
    x3f_directory_entry_header_t *DEH = &DE->header;
    x3f_image_data_t *ID = &DEH->data_subsection.image_data;
    if (!ID)
      throw LIBRAW_EXCEPTION_IO_CORRUPT;
    x3f_quattro_t *Q = ID->quattro;
    x3f_huffman_t *HUF = ID->huffman;
    x3f_true_t *TRU = ID->tru;
    uint16_t *data = NULL;
    if (ID->rows != S.raw_height || ID->columns != S.raw_width)
    {
      raise_error = 1;
      goto end;
    }
    if (HUF != NULL)
      data = HUF->x3rgb16.data;
    if (TRU != NULL)
      data = TRU->x3rgb16.data;
    if (data == NULL)
    {
      raise_error = 1;
      goto end;
    }

    size_t datasize = S.raw_height * S.raw_width * 3 * sizeof(unsigned short);
    S.raw_pitch = S.raw_width * 3 * sizeof(unsigned short);
    if (!(imgdata.rawdata.raw_alloc = malloc(datasize)))
      throw LIBRAW_EXCEPTION_ALLOC;

    imgdata.rawdata.color3_image = (ushort(*)[3])imgdata.rawdata.raw_alloc;
    // swap R/B channels for known old cameras
    if (!strcasecmp(P1.make, "Polaroid") && !strcasecmp(P1.model, "x530"))
    {
      ushort(*src)[3] = (ushort(*)[3])data;
      for (int p = 0; p < S.raw_height * S.raw_width; p++)
      {
        imgdata.rawdata.color3_image[p][0] = src[p][2];
        imgdata.rawdata.color3_image[p][1] = src[p][1];
        imgdata.rawdata.color3_image[p][2] = src[p][0];
      }
    }
    else if (HUF)
      memmove(imgdata.rawdata.raw_alloc, data, datasize);
    else if (TRU && (!Q || !Q->quattro_layout))
      memmove(imgdata.rawdata.raw_alloc, data, datasize);
    else if (TRU && Q)
    {
      // Move quattro data in place
      // R/B plane
      for (int prow = 0; prow < TRU->x3rgb16.rows && prow < S.raw_height / 2;
           prow++)
      {
        ushort(*destrow)[3] =
            (unsigned short(*)[3]) &
            imgdata.rawdata
                .color3_image[prow * 2 * S.raw_pitch / 3 / sizeof(ushort)][0];
        ushort(*srcrow)[3] =
            (unsigned short(*)[3]) & data[prow * TRU->x3rgb16.row_stride];
        for (int pcol = 0;
             pcol < TRU->x3rgb16.columns && pcol < S.raw_width / 2; pcol++)
        {
          destrow[pcol * 2][0] = srcrow[pcol][0];
          destrow[pcol * 2][1] = srcrow[pcol][1];
        }
      }
      for (int row = 0; row < Q->top16.rows && row < S.raw_height; row++)
      {
        ushort(*destrow)[3] =
            (unsigned short(*)[3]) &
            imgdata.rawdata
                .color3_image[row * S.raw_pitch / 3 / sizeof(ushort)][0];
        ushort(*srcrow) =
            (unsigned short *)&Q->top16.data[row * Q->top16.columns];
        for (int col = 0; col < Q->top16.columns && col < S.raw_width; col++)
          destrow[col][2] = srcrow[col];
      }
    }

#if 1
    if (TRU && Q &&
        (imgdata.params.raw_processing_options &
         LIBRAW_PROCESSING_DP2Q_INTERPOLATEAF))
    {
      if (imgdata.sizes.raw_width == 5888 &&
          imgdata.sizes.raw_height == 3672) // dpN Quattro normal
      {
        x3f_dpq_interpolate_af(32, 8, 2);
      }
      else if (imgdata.sizes.raw_width == 5888 &&
               imgdata.sizes.raw_height == 3776) // sd Quattro normal raw
      {
        x3f_dpq_interpolate_af_sd(216, 464, imgdata.sizes.raw_width - 1, 3312,
                                  16, 32, 2);
      }
      else if (imgdata.sizes.raw_width == 6656 &&
               imgdata.sizes.raw_height == 4480) // sd Quattro H normal raw
      {
        x3f_dpq_interpolate_af_sd(232, 592, imgdata.sizes.raw_width - 1, 3920,
                                  16, 32, 2);
      }
      else if (imgdata.sizes.raw_width == 3328 &&
               imgdata.sizes.raw_height == 2240) // sd Quattro H half size
      {
        x3f_dpq_interpolate_af_sd(116, 296, imgdata.sizes.raw_width - 1, 2200,
                                  8, 16, 1);
      }
      else if (imgdata.sizes.raw_width == 5504 &&
               imgdata.sizes.raw_height == 3680) // sd Quattro H APS-C raw
      {
        x3f_dpq_interpolate_af_sd(8, 192, imgdata.sizes.raw_width - 1, 3185, 16,
                                  32, 2);
      }
      else if (imgdata.sizes.raw_width == 2752 &&
               imgdata.sizes.raw_height == 1840) // sd Quattro H APS-C half size
      {
        x3f_dpq_interpolate_af_sd(4, 96, imgdata.sizes.raw_width - 1, 1800, 8,
                                  16, 1);
      }
      else if (imgdata.sizes.raw_width == 2944 &&
               imgdata.sizes.raw_height == 1836) // dpN Quattro small raw
      {
        x3f_dpq_interpolate_af(16, 4, 1);
      }
      else if (imgdata.sizes.raw_width == 2944 &&
               imgdata.sizes.raw_height == 1888) // sd Quattro small
      {
        x3f_dpq_interpolate_af_sd(108, 232, imgdata.sizes.raw_width - 1, 1656,
                                  8, 16, 1);
      }
    }
#endif
    if (TRU && Q && Q->quattro_layout &&
        (imgdata.params.raw_processing_options &
         LIBRAW_PROCESSING_DP2Q_INTERPOLATERG))
      x3f_dpq_interpolate_rg();
  }
  else
    raise_error = 1;
end:
  if (raise_error)
    throw LIBRAW_EXCEPTION_IO_CORRUPT;
}