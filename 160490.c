  DllDef void libraw_set_output_color(libraw_data_t *lr, int value)
  {
    if (!lr)
      return;
    LibRaw *ip = (LibRaw *)lr->parent_class;
    ip->imgdata.params.output_color = value;
  }