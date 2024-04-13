void LibRaw::pentax_4shot_load_raw()
{
  ushort *plane = (ushort *)malloc(imgdata.sizes.raw_width *
                                   imgdata.sizes.raw_height * sizeof(ushort));
  int alloc_sz = imgdata.sizes.raw_width * (imgdata.sizes.raw_height + 16) * 4 *
                 sizeof(ushort);
  ushort(*result)[4] = (ushort(*)[4])malloc(alloc_sz);
  struct movement_t
  {
    int row, col;
  } _move[4] = {
      {1, 1},
      {0, 1},
      {0, 0},
      {1, 0},
  };

  int tidx = 0;
  for (int i = 0; i < 4; i++)
  {
    int move_row, move_col;
    if (imgdata.params.p4shot_order[i] >= '0' &&
        imgdata.params.p4shot_order[i] <= '3')
    {
      move_row = (imgdata.params.p4shot_order[i] - '0' & 2) ? 1 : 0;
      move_col = (imgdata.params.p4shot_order[i] - '0' & 1) ? 1 : 0;
    }
    else
    {
      move_row = _move[i].row;
      move_col = _move[i].col;
    }
    for (; tidx < 16; tidx++)
      if (tiff_ifd[tidx].t_width == imgdata.sizes.raw_width &&
          tiff_ifd[tidx].t_height == imgdata.sizes.raw_height &&
          tiff_ifd[tidx].bps > 8 && tiff_ifd[tidx].samples == 1)
        break;
    if (tidx >= 16)
      break;
    imgdata.rawdata.raw_image = plane;
    ID.input->seek(tiff_ifd[tidx].offset, SEEK_SET);
    imgdata.idata.filters = 0xb4b4b4b4;
    libraw_internal_data.unpacker_data.data_offset = tiff_ifd[tidx].offset;
    (this->*pentax_component_load_raw)();
    for (int row = 0; row < imgdata.sizes.raw_height - move_row; row++)
    {
      int colors[2];
      for (int c = 0; c < 2; c++)
        colors[c] = COLOR(row, c);
      ushort *srcrow = &plane[imgdata.sizes.raw_width * row];
      ushort(*dstrow)[4] =
          &result[(imgdata.sizes.raw_width) * (row + move_row) + move_col];
      for (int col = 0; col < imgdata.sizes.raw_width - move_col; col++)
        dstrow[col][colors[col % 2]] = srcrow[col];
    }
    tidx++;
  }

  if (imgdata.color.cblack[4] == 2 && imgdata.color.cblack[5] == 2)
    for (int c = 0; c < 4; c++)
      imgdata.color.cblack[FC(c / 2, c % 2)] +=
          imgdata.color.cblack[6 +
                               c / 2 % imgdata.color.cblack[4] *
                                   imgdata.color.cblack[5] +
                               c % 2 % imgdata.color.cblack[5]];
  imgdata.color.cblack[4] = imgdata.color.cblack[5] = 0;

  // assign things back:
  imgdata.sizes.raw_pitch = imgdata.sizes.raw_width * 8;
  imgdata.idata.filters = 0;
  imgdata.rawdata.raw_alloc = imgdata.rawdata.color4_image = result;
  free(plane);
  imgdata.rawdata.raw_image = 0;
}