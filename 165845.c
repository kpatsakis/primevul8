gst_mpegts_pat_new (void)
{
  GPtrArray *pat;

  pat = g_ptr_array_new_with_free_func (
      (GDestroyNotify) _mpegts_pat_program_free);

  return pat;
}