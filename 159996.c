void LibRaw::kodak_jpeg_load_raw()
{
  if (data_size < 1)
    throw LIBRAW_EXCEPTION_DECODE_JPEG;

  int row, col;
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr pub;
  cinfo.err = jpeg_std_error(&pub);
  pub.error_exit = jpegErrorExit_k;

  unsigned char *jpg_buf = (unsigned char *)malloc(data_size);
  merror(jpg_buf, "kodak_jpeg_load_raw");
  unsigned char *pixel_buf = (unsigned char *)malloc(width * 3);
  jpeg_create_decompress(&cinfo);
  merror(pixel_buf, "kodak_jpeg_load_raw");

  fread(jpg_buf, data_size, 1, ifp);
  swab((char *)jpg_buf, (char *)jpg_buf, data_size);
  try
  {
    jpeg_mem_src(&cinfo, jpg_buf, data_size);
    int rc = jpeg_read_header(&cinfo, TRUE);
    if (rc != 1)
      throw LIBRAW_EXCEPTION_DECODE_JPEG;

    jpeg_start_decompress(&cinfo);
    if ((cinfo.output_width != width) || (cinfo.output_height * 2 != height) ||
        (cinfo.output_components != 3))
    {
      throw LIBRAW_EXCEPTION_DECODE_JPEG;
    }

    unsigned char *buf[1];
    buf[0] = pixel_buf;

    while (cinfo.output_scanline < cinfo.output_height)
    {
      checkCancel();
      row = cinfo.output_scanline * 2;
      jpeg_read_scanlines(&cinfo, buf, 1);
      unsigned char(*pixel)[3] = (unsigned char(*)[3])buf[0];
      for (col = 0; col < width; col += 2)
      {
        RAW(row + 0, col + 0) = pixel[col + 0][1] << 1;
        RAW(row + 1, col + 1) = pixel[col + 1][1] << 1;
        RAW(row + 0, col + 1) = pixel[col][0] + pixel[col + 1][0];
        RAW(row + 1, col + 0) = pixel[col][2] + pixel[col + 1][2];
      }
    }
  }
  catch (...)
  {
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    free(jpg_buf);
    free(pixel_buf);
    throw;
  }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  free(jpg_buf);
  free(pixel_buf);
  maximum = 0xff << 1;
}