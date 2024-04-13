void LibRaw::ahd_interpolate()
{
  int i, j, k, top, left;
  float xyz_cam[3][4], r;
  char *buffer;
  ushort(*rgb)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3];
  short(*lab)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3];
  char(*homo)[LIBRAW_AHD_TILE][2];
  int terminate_flag = 0;

  cielab(0, 0);
  border_interpolate(5);

#ifdef LIBRAW_USE_OPENMP
#pragma omp parallel private(buffer, rgb, lab, homo, top, left, i, j, k)       \
    shared(xyz_cam, terminate_flag)
#endif
  {
#ifdef LIBRAW_USE_OPENMP
#pragma omp critical
#endif
    buffer =
        (char *)malloc(26 * LIBRAW_AHD_TILE * LIBRAW_AHD_TILE); /* 1664 kB */
    merror(buffer, "ahd_interpolate()");
    rgb = (ushort(*)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3])buffer;
    lab = (short(*)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3])(
        buffer + 12 * LIBRAW_AHD_TILE * LIBRAW_AHD_TILE);
    homo = (char(*)[LIBRAW_AHD_TILE][2])(buffer + 24 * LIBRAW_AHD_TILE *
                                                      LIBRAW_AHD_TILE);

#ifdef LIBRAW_USE_OPENMP
#pragma omp for schedule(dynamic)
#endif
    for (top = 2; top < height - 5; top += LIBRAW_AHD_TILE - 6)
    {
#ifdef LIBRAW_USE_OPENMP
      if (0 == omp_get_thread_num())
#endif
        if (callbacks.progress_cb)
        {
          int rr = (*callbacks.progress_cb)(callbacks.progresscb_data,
                                            LIBRAW_PROGRESS_INTERPOLATE,
                                            top - 2, height - 7);
          if (rr)
            terminate_flag = 1;
        }
      for (left = 2; !terminate_flag && (left < width - 5);
           left += LIBRAW_AHD_TILE - 6)
      {
        ahd_interpolate_green_h_and_v(top, left, rgb);
        ahd_interpolate_r_and_b_and_convert_to_cielab(top, left, rgb, lab);
        ahd_interpolate_build_homogeneity_map(top, left, lab, homo);
        ahd_interpolate_combine_homogeneous_pixels(top, left, rgb, homo);
      }
    }
#ifdef LIBRAW_USE_OPENMP
#pragma omp critical
#endif
    free(buffer);
  }
  if (terminate_flag)
    throw LIBRAW_EXCEPTION_CANCELLED_BY_CALLBACK;
}