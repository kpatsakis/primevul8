void LibRaw::fuji_compressed_load_raw()
{
  struct fuji_compressed_params common_info;
  int cur_block;
  unsigned line_size, *block_sizes;
  INT64 raw_offset, *raw_block_offsets;
  // struct fuji_compressed_block info;

  init_fuji_compr(&common_info);
  line_size = sizeof(ushort) * (common_info.line_width + 2);

  // read block sizes
  block_sizes = (unsigned *)malloc(
      sizeof(unsigned) * libraw_internal_data.unpacker_data.fuji_total_blocks);
  merror(block_sizes, "fuji_compressed_load_raw()");
  raw_block_offsets = (INT64 *)malloc(
      sizeof(INT64) * libraw_internal_data.unpacker_data.fuji_total_blocks);
  merror(raw_block_offsets, "fuji_compressed_load_raw()");

  raw_offset =
      sizeof(unsigned) * libraw_internal_data.unpacker_data.fuji_total_blocks;
  if (raw_offset & 0xC)
    raw_offset += 0x10 - (raw_offset & 0xC);

  raw_offset += libraw_internal_data.unpacker_data.data_offset;

  libraw_internal_data.internal_data.input->seek(
      libraw_internal_data.unpacker_data.data_offset, SEEK_SET);
  libraw_internal_data.internal_data.input->read(
      block_sizes, 1,
      sizeof(unsigned) * libraw_internal_data.unpacker_data.fuji_total_blocks);

  raw_block_offsets[0] = raw_offset;
  // calculating raw block offsets
  for (cur_block = 0;
       cur_block < libraw_internal_data.unpacker_data.fuji_total_blocks;
       cur_block++)
  {
    unsigned bsize = sgetn(4, (uchar *)(block_sizes + cur_block));
    block_sizes[cur_block] = bsize;
  }

  for (cur_block = 1;
       cur_block < libraw_internal_data.unpacker_data.fuji_total_blocks;
       cur_block++)
    raw_block_offsets[cur_block] =
        raw_block_offsets[cur_block - 1] + block_sizes[cur_block - 1];

  fuji_decode_loop(&common_info,
                   libraw_internal_data.unpacker_data.fuji_total_blocks,
                   raw_block_offsets, block_sizes);

  free(block_sizes);
  free(raw_block_offsets);
  free(common_info.q_table);
}