void LibRaw::unpacked_load_raw_fuji_f700s20()
{
  int base_offset = 0;
  int row_size = imgdata.sizes.raw_width * 2; // in bytes
  if (imgdata.idata.raw_count == 2 && imgdata.params.shot_select)
  {
    libraw_internal_data.internal_data.input->seek(-row_size, SEEK_CUR);
    base_offset = row_size; // in bytes
  }
  unsigned char *buffer = (unsigned char *)malloc(row_size * 2);
  for (int row = 0; row < imgdata.sizes.raw_height; row++)
  {
    read_shorts((ushort *)buffer, imgdata.sizes.raw_width * 2);
    memmove(&imgdata.rawdata.raw_image[row * imgdata.sizes.raw_pitch / 2],
            buffer + base_offset, row_size);
  }
  free(buffer);
}