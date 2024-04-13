read_byte (tga_source_ptr sinfo)
/* Read next byte from Targa file */
{
  register FILE *infile = sinfo->pub.input_file;
  register int c;

  if ((c = getc(infile)) == EOF)
    ERREXIT(sinfo->cinfo, JERR_INPUT_EOF);
  return c;
}