fribidi_set_mirroring (
  /* input */
  fribidi_boolean state
)
{
  return FRIBIDI_ADJUST_AND_TEST_BITS (flags, FRIBIDI_FLAG_SHAPE_MIRRORING, state);
}