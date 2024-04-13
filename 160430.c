void LibRaw::parse_fuji_compressed_header()
{
  unsigned signature, version, h_raw_type, h_raw_bits, h_raw_height,
      h_raw_rounded_width, h_raw_width, h_block_size, h_blocks_in_row,
      h_total_lines;

  uchar header[16];

  libraw_internal_data.internal_data.input->seek(
      libraw_internal_data.unpacker_data.data_offset, SEEK_SET);
  libraw_internal_data.internal_data.input->read(header, 1, sizeof(header));

  // read all header
  signature = sgetn(2, header);
  version = header[2];
  h_raw_type = header[3];
  h_raw_bits = header[4];
  h_raw_height = sgetn(2, header + 5);
  h_raw_rounded_width = sgetn(2, header + 7);
  h_raw_width = sgetn(2, header + 9);
  h_block_size = sgetn(2, header + 11);
  h_blocks_in_row = header[13];
  h_total_lines = sgetn(2, header + 14);

  // general validation
  if (signature != 0x4953 || version != 1 || h_raw_height > 0x3000 ||
      h_raw_height < 6 || h_raw_height % 6 || h_block_size < 1 ||
      h_raw_width > 0x3000 || h_raw_width < 0x300 || h_raw_width % 24 ||
      h_raw_rounded_width > 0x3000 || h_raw_rounded_width < h_block_size ||
      h_raw_rounded_width % h_block_size ||
      h_raw_rounded_width - h_raw_width >= h_block_size ||
      h_block_size != 0x300 || h_blocks_in_row > 0x10 || h_blocks_in_row == 0 ||
      h_blocks_in_row != h_raw_rounded_width / h_block_size ||
      h_total_lines > 0x800 || h_total_lines == 0 ||
      h_total_lines != h_raw_height / 6 ||
      (h_raw_bits != 12 && h_raw_bits != 14 && h_raw_bits != 16) ||
      (h_raw_type != 16 && h_raw_type != 0))
    return;

  // modify data
  libraw_internal_data.unpacker_data.fuji_total_lines = h_total_lines;
  libraw_internal_data.unpacker_data.fuji_total_blocks = h_blocks_in_row;
  libraw_internal_data.unpacker_data.fuji_block_width = h_block_size;
  libraw_internal_data.unpacker_data.fuji_bits = h_raw_bits;
  libraw_internal_data.unpacker_data.fuji_raw_type = h_raw_type;
  imgdata.sizes.raw_width = h_raw_width;
  imgdata.sizes.raw_height = h_raw_height;
  libraw_internal_data.unpacker_data.data_offset += 16;
  load_raw = &LibRaw::fuji_compressed_load_raw;
}