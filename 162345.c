jinit_read_ppm(j_compress_ptr cinfo)
{
  ppm_source_ptr source;

  /* Create module interface object */
  source = (ppm_source_ptr)
    (*cinfo->mem->alloc_small) ((j_common_ptr)cinfo, JPOOL_IMAGE,
                                sizeof(ppm_source_struct));
  /* Fill in method ptrs, except get_pixel_rows which start_input sets */
  source->pub.start_input = start_input_ppm;
  source->pub.finish_input = finish_input_ppm;

  return (cjpeg_source_ptr)source;
}