void LibRaw::fbdd(int noiserd)
{
  double(*image2)[3];
  // safety net: disable for 4-color bayer or full-color images
  if (colors != 3 || !filters)
    return;
  image2 = (double(*)[3])calloc(width * height, sizeof *image2);

  border_interpolate(4);

  if (noiserd > 1)
  {
    fbdd_green();
    // dcb_color_full(image2);
    dcb_color_full();
    fbdd_correction();

    dcb_color();
    rgb_to_lch(image2);
    fbdd_correction2(image2);
    fbdd_correction2(image2);
    lch_to_rgb(image2);
  }
  else
  {
    fbdd_green();
    // dcb_color_full(image2);
    dcb_color_full();
    fbdd_correction();
  }

  free(image2);
}