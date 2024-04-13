preload_image (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  tga_source_ptr source = (tga_source_ptr) sinfo;
  JDIMENSION row;
  cd_progress_ptr progress = (cd_progress_ptr) cinfo->progress;

  /* Read the data into a virtual array in input-file row order. */
  for (row = 0; row < cinfo->image_height; row++) {
    if (progress != NULL) {
      progress->pub.pass_counter = (long) row;
      progress->pub.pass_limit = (long) cinfo->image_height;
      (*progress->pub.progress_monitor) ((j_common_ptr) cinfo);
    }
    source->pub.buffer = (*cinfo->mem->access_virt_sarray)
      ((j_common_ptr) cinfo, source->whole_image, row, (JDIMENSION) 1, TRUE);
    (*source->get_pixel_rows) (cinfo, sinfo);
  }
  if (progress != NULL)
    progress->completed_extra_passes++;

  /* Set up to read from the virtual array in unscrambled order */
  source->pub.get_pixel_rows = get_memory_row;
  source->current_row = 0;
  /* And read the first row */
  return get_memory_row(cinfo, sinfo);
}