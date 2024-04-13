int LibRaw::is_coolscan_nef()
{
  return load_raw == &LibRaw::nikon_coolscan_load_raw;
}