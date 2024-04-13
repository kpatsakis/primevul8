void LibRaw::fuji_decode_loop(const struct fuji_compressed_params *common_info,
                              int count, INT64 *raw_block_offsets,
                              unsigned *block_sizes)
{
  int cur_block;
#ifdef LIBRAW_USE_OPENMP
#pragma omp parallel for private(cur_block)
#endif
  for (cur_block = 0; cur_block < count; cur_block++)
  {
    fuji_decode_strip(common_info, cur_block, raw_block_offsets[cur_block],
                      block_sizes[cur_block]);
  }
}