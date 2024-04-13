void LibRaw::sony_arq_load_raw()
{
  int row, col;
  read_shorts(imgdata.rawdata.raw_image,
              imgdata.sizes.raw_width * imgdata.sizes.raw_height * 4);
  libraw_internal_data.internal_data.input->seek(
      -2, SEEK_CUR); // avoid wrong eof error
  for (row = 0; row < imgdata.sizes.raw_height; row++)
  {
    unsigned short(*rowp)[4] =
        (unsigned short(*)[4]) &
        imgdata.rawdata.raw_image[row * imgdata.sizes.raw_width * 4];
    for (col = 0; col < imgdata.sizes.raw_width; col++)
    {
      unsigned short g2 = rowp[col][2];
      rowp[col][2] = rowp[col][3];
      rowp[col][3] = g2;
      if (((unsigned)(row - imgdata.sizes.top_margin) < imgdata.sizes.height) &&
          ((unsigned)(col - imgdata.sizes.left_margin) < imgdata.sizes.width) &&
          (MAX(MAX(rowp[col][0], rowp[col][1]),
               MAX(rowp[col][2], rowp[col][3])) > imgdata.color.maximum))
        derror();
    }
  }
}