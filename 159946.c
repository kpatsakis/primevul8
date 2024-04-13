void LibRaw::ahd_interpolate_r_and_b_and_convert_to_cielab(
    int top, int left, ushort (*inout_rgb)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3],
    short (*out_lab)[LIBRAW_AHD_TILE][LIBRAW_AHD_TILE][3])
{
  int direction;
  for (direction = 0; direction < 2; direction++)
  {
    ahd_interpolate_r_and_b_in_rgb_and_convert_to_cielab(
        top, left, inout_rgb[direction], out_lab[direction]);
  }
}