void LibRaw::recycle_datastream()
{
  if (libraw_internal_data.internal_data.input &&
      libraw_internal_data.internal_data.input_internal)
  {
    delete libraw_internal_data.internal_data.input;
    libraw_internal_data.internal_data.input = NULL;
  }
  libraw_internal_data.internal_data.input_internal = 0;
}