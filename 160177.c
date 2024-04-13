void pana_cs6_page_decoder::read_page()
{
  if (!buffer || (maxoffset - lastoffset < 16))
    throw LIBRAW_EXCEPTION_IO_EOF;
#define wbuffer(i) ((unsigned short)buffer[lastoffset + 15 - i])
  pixelbuffer[0] = (wbuffer(0) << 6) | (wbuffer(1) >> 2); // 14 bit
  pixelbuffer[1] =
      (((wbuffer(1) & 0x3) << 12) | (wbuffer(2) << 4) | (wbuffer(3) >> 4)) &
      0x3fff;
  pixelbuffer[2] = (wbuffer(3) >> 2) & 0x3;
  pixelbuffer[3] = ((wbuffer(3) & 0x3) << 8) | wbuffer(4);
  pixelbuffer[4] = (wbuffer(5) << 2) | (wbuffer(6) >> 6);
  pixelbuffer[5] = ((wbuffer(6) & 0x3f) << 4) | (wbuffer(7) >> 4);
  pixelbuffer[6] = (wbuffer(7) >> 2) & 0x3;
  pixelbuffer[7] = ((wbuffer(7) & 0x3) << 8) | wbuffer(8);
  pixelbuffer[8] = ((wbuffer(9) << 2) & 0x3fc) | (wbuffer(10) >> 6);
  pixelbuffer[9] = ((wbuffer(10) << 4) | (wbuffer(11) >> 4)) & 0x3ff;
  pixelbuffer[10] = (wbuffer(11) >> 2) & 0x3;
  pixelbuffer[11] = ((wbuffer(11) & 0x3) << 8) | wbuffer(12);
  pixelbuffer[12] = (((wbuffer(13) << 2) & 0x3fc) | wbuffer(14) >> 6) & 0x3ff;
  pixelbuffer[13] = ((wbuffer(14) << 4) | (wbuffer(15) >> 4)) & 0x3ff;
#undef wbuffer
  current = 0;
  lastoffset += 16;
}