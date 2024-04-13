void LibRaw::dcb(int iterations, int dcb_enhance)
{

  int i = 1;

  float(*image2)[3];
  image2 = (float(*)[3])calloc(width * height, sizeof *image2);

  float(*image3)[3];
  image3 = (float(*)[3])calloc(width * height, sizeof *image3);

  border_interpolate(6);

  dcb_hor(image2);
  dcb_color2(image2);

  dcb_ver(image3);
  dcb_color3(image3);

  dcb_decide(image2, image3);

  free(image3);

  dcb_copy_to_buffer(image2);

  while (i <= iterations)
  {
    dcb_nyquist();
    dcb_nyquist();
    dcb_nyquist();
    dcb_map();
    dcb_correction();
    i++;
  }

  dcb_color();
  dcb_pp();

  dcb_map();
  dcb_correction2();

  dcb_map();
  dcb_correction();

  dcb_map();
  dcb_correction();

  dcb_map();
  dcb_correction();

  dcb_map();
  dcb_restore_from_buffer(image2);
  dcb_color();

  if (dcb_enhance)
  {
    dcb_refinement();
    // dcb_color_full(image2);
    dcb_color_full();
  }

  free(image2);
}