get_raw_row(j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading raw-byte-format files with maxval = MAXJSAMPLE.
 * In this case we just read right into the JSAMPLE buffer!
 * Note that same code works for PPM and PGM files.
 */
{
  ppm_source_ptr source = (ppm_source_ptr)sinfo;

  if (!ReadOK(source->pub.input_file, source->iobuffer, source->buffer_width))
    ERREXIT(cinfo, JERR_INPUT_EOF);
  return 1;
}