void LibRaw::aahd_interpolate()
{
  AAHD aahd(*this);
  aahd.hide_hots();
  aahd.make_ahd_greens();
  aahd.make_ahd_rb();
  aahd.evaluate_ahd();
  aahd.refine_hv_dirs();
  //	aahd.illustrate_dirs();
  aahd.combine_image();
}