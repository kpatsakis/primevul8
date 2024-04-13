get_24bit_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
/* This version is for reading 24-bit pixels */
{
  tga_source_ptr source = (tga_source_ptr) sinfo;
  register JSAMPROW ptr;
  register JDIMENSION col;

  ptr = source->pub.buffer[0];
  for (col = cinfo->image_width; col > 0; col--) {
    (*source->read_pixel) (source); /* Load next pixel into tga_pixel */
    *ptr++ = (JSAMPLE) UCH(source->tga_pixel[2]); /* change BGR to RGB order */
    *ptr++ = (JSAMPLE) UCH(source->tga_pixel[1]);
    *ptr++ = (JSAMPLE) UCH(source->tga_pixel[0]);
  }
  return 1;
}