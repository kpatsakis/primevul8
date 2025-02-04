static void splashOutBlendSaturation(SplashColorPtr src, SplashColorPtr dest,
				     SplashColorPtr blend,
				     SplashColorMode cm) {
  unsigned char r0, g0, b0;
#ifdef SPLASH_CMYK
  unsigned char r1, g1, b1;
  int i;
  SplashColor src2, dest2;
#endif

  switch (cm) {
  case splashModeMono1:
  case splashModeMono8:
    blend[0] = dest[0];
    break;
  case splashModeXBGR8:
    src[3] = 255;
    // fallthrough
  case splashModeRGB8:
  case splashModeBGR8:
    setSat(dest[0], dest[1], dest[2], getSat(src[0], src[1], src[2]),
	   &r0, &g0, &b0);
    setLum(r0, g0, b0, getLum(dest[0], dest[1], dest[2]),
	   &blend[0], &blend[1], &blend[2]);
    break;
#ifdef SPLASH_CMYK
  case splashModeCMYK8:
  case splashModeDeviceN8:
    for (i = 0; i < 4; i++) {
      // convert to additive
      src2[i] = 0xff - src[i];
      dest2[i] = 0xff - dest[i];
    }
    setSat(dest2[0], dest2[1], dest2[2], getSat(src2[0], src2[1], src2[2]),
	   &r0, &g0, &b0);
    setLum(r0, g0, b0, getLum(dest2[0], dest2[1], dest2[2]),
	   &r1, &g1, &b1);
    blend[0] = r1;
    blend[1] = g1;
    blend[2] = b1;
    blend[3] = dest2[3];
    for (i = 0; i < 4; i++) {
      // convert back to subtractive
      blend[i] = 0xff - blend[i];
    }
    break;
#endif
  }
}