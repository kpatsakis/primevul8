  pana_cs6_page_decoder(unsigned char *_buffer, unsigned int bsize)
      : lastoffset(0), maxoffset(bsize), current(0), buffer(_buffer)
  {
  }