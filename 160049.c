void LibRaw::panasonicC6_load_raw()
{
  const int rowstep = 16;
  const int blocksperrow = imgdata.sizes.raw_width / 11;
  const int rowbytes = blocksperrow * 16;
  unsigned char *iobuf = (unsigned char *)malloc(rowbytes * rowstep);
  merror(iobuf, "panasonicC6_load_raw()");

  for (int row = 0; row < imgdata.sizes.raw_height - rowstep + 1;
       row += rowstep)
  {
    int rowstoread = MIN(rowstep, imgdata.sizes.raw_height - row);
    if (libraw_internal_data.internal_data.input->read(
            iobuf, rowbytes, rowstoread) != rowstoread)
      throw LIBRAW_EXCEPTION_IO_EOF;
    pana_cs6_page_decoder page(iobuf, rowbytes * rowstoread);
    for (int crow = 0, col = 0; crow < rowstoread; crow++, col = 0)
    {
      unsigned short *rowptr =
          &imgdata.rawdata
               .raw_image[(row + crow) * imgdata.sizes.raw_pitch / 2];
      for (int rblock = 0; rblock < blocksperrow; rblock++)
      {
        page.read_page();
        unsigned oddeven[2] = {0, 0}, nonzero[2] = {0, 0};
        unsigned pmul = 0, pixel_base = 0;
        for (int pix = 0; pix < 11; pix++)
        {
          if (pix % 3 == 2)
          {
            unsigned base = page.nextpixel();
            if (base > 3)
              throw LIBRAW_EXCEPTION_IO_CORRUPT; // not possible b/c of 2-bit
                                                 // field, but....
            if (base == 3)
              base = 4;
            pixel_base = 0x200 << base;
            pmul = 1 << base;
          }
          unsigned epixel = page.nextpixel();
          if (oddeven[pix % 2])
          {
            epixel *= pmul;
            if (pixel_base < 0x2000 && nonzero[pix % 2] > pixel_base)
              epixel += nonzero[pix % 2] - pixel_base;
            nonzero[pix % 2] = epixel;
          }
          else
          {
            oddeven[pix % 2] = epixel;
            if (epixel)
              nonzero[pix % 2] = epixel;
            else
              epixel = nonzero[pix % 2];
          }
          unsigned spix = epixel - 0xf;
          if (spix <= 0xffff)
            rowptr[col++] = spix & 0xffff;
          else
          {
            epixel = (((signed int)(epixel + 0x7ffffff1)) >> 0x1f);
            rowptr[col++] = epixel & 0x3fff;
          }
        }
      }
    }
  }
  free(iobuf);
}