_mpegts_pat_program_copy (GstMpegtsPatProgram * orig)
{
  return g_slice_dup (GstMpegtsPatProgram, orig);
}