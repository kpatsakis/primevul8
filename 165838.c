gst_mpegts_pat_program_new (void)
{
  GstMpegtsPatProgram *program;

  program = g_slice_new0 (GstMpegtsPatProgram);

  return program;
}