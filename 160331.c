int LibRaw::is_phaseone_compressed()
{
  return (load_raw == &LibRaw::phase_one_load_raw_c ||
          load_raw == &LibRaw::phase_one_load_raw);
}