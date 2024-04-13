fribidi_set_reorder_nsm (
  /* input */
  fribidi_boolean state
)
{
  return FRIBIDI_ADJUST_AND_TEST_BITS (flags, FRIBIDI_FLAG_REORDER_NSM, state);
}