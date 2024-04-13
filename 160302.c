  int libraw_dcraw_ppm_tiff_writer(libraw_data_t *lr, const char *filename)
  {
    if (!lr)
      return EINVAL;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    return ip->dcraw_ppm_tiff_writer(filename);
  }