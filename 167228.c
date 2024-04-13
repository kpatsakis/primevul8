jinit_read_targa (j_compress_ptr cinfo)
{
  tga_source_ptr source;

  /* Create module interface object */
  source = (tga_source_ptr)
      (*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_IMAGE,
                                  sizeof(tga_source_struct));
  source->cinfo = cinfo;        /* make back link for subroutines */
  /* Fill in method ptrs, except get_pixel_rows which start_input sets */
  source->pub.start_input = start_input_tga;
  source->pub.finish_input = finish_input_tga;

  return (cjpeg_source_ptr) source;
}