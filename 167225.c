get_memory_row (j_compress_ptr cinfo, cjpeg_source_ptr sinfo)
{
  tga_source_ptr source = (tga_source_ptr) sinfo;
  JDIMENSION source_row;

  /* Compute row of source that maps to current_row of normal order */
  /* For now, assume image is bottom-up and not interlaced. */
  /* NEEDS WORK to support interlaced images! */
  source_row = cinfo->image_height - source->current_row - 1;

  /* Fetch that row from virtual array */
  source->pub.buffer = (*cinfo->mem->access_virt_sarray)
    ((j_common_ptr) cinfo, source->whole_image,
     source_row, (JDIMENSION) 1, FALSE);

  source->current_row++;
  return 1;
}