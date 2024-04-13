void LibRaw::parse_broadcom()
{

  /* This structure is at offset 0xb0 from the 'BRCM' ident. */
  struct
  {
    uint8_t umode[32];
    uint16_t uwidth;
    uint16_t uheight;
    uint16_t padding_right;
    uint16_t padding_down;
    uint32_t unknown_block[6];
    uint16_t transform;
    uint16_t format;
    uint8_t bayer_order;
    uint8_t bayer_format;
  } header;

  header.bayer_order = 0;
  fseek(ifp, 0xb0 - 0x20, SEEK_CUR);
  fread(&header, 1, sizeof(header), ifp);
  /* load_flags is not used in broadcom loader, so reuse it for raw_stride */
  load_flags =
      ((((((header.uwidth + header.padding_right) * 5) + 3) >> 2) + 0x1f) &
       (~0x1f));
  raw_width = width = header.uwidth;
  raw_height = height = header.uheight;
  filters = 0x16161616; /* default Bayer order is 2, BGGR */

  switch (header.bayer_order)
  {
  case 0: /* RGGB */
    filters = 0x94949494;
    break;
  case 1: /* GBRG */
    filters = 0x49494949;
    break;
  case 3: /* GRBG */
    filters = 0x61616161;
    break;
  }
}