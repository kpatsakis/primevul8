bool PrimaryChromaticity::Parse(IMkvReader* reader, long long read_pos,
                                long long value_size, bool is_x,
                                PrimaryChromaticity** chromaticity) {
  if (!reader)
    return false;

  if (!*chromaticity)
    *chromaticity = new PrimaryChromaticity();

  if (!*chromaticity)
    return false;

  PrimaryChromaticity* pc = *chromaticity;
  float* value = is_x ? &pc->x : &pc->y;

  double parser_value = 0;
  const long long parse_status =
      UnserializeFloat(reader, read_pos, value_size, parser_value);

  // Valid range is [0, 1]. Make sure the double is representable as a float
  // before casting.
  if (parse_status < 0 || parser_value < 0.0 || parser_value > 1.0 ||
      (parser_value > 0.0 && parser_value < FLT_MIN))
    return false;

  *value = static_cast<float>(parser_value);

  return true;
}