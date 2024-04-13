int LibRaw::is_sraw()
{
  return load_raw == &LibRaw::canon_sraw_load_raw ||
         load_raw == &LibRaw::nikon_load_sraw;
}