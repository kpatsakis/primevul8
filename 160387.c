void LibRaw::unpacked_load_raw_FujiDBP()
/*
for Fuji DBP for GX680, aka DX-2000
  DBP_tile_width = 688;
  DBP_tile_height = 3856;
  DBP_n_tiles = 8;
*/
{
  int scan_line, tile_n;
  int nTiles;

  nTiles = 8;
  tile_width = raw_width / nTiles;

  ushort *tile;
  tile = (ushort *)calloc(raw_height, tile_width * 2);

  for (tile_n = 0; tile_n < nTiles; tile_n++)
  {
    read_shorts(tile, tile_width * raw_height);
    for (scan_line = 0; scan_line < raw_height; scan_line++)
    {
      memcpy(&raw_image[scan_line * raw_width + tile_n * tile_width],
             &tile[scan_line * tile_width], tile_width * 2);
    }
  }
  free(tile);
  fseek(ifp, -2, SEEK_CUR); // avoid EOF error
}