set_no_icc(fz_colorspace_context *cct)
{
	cct->gray = default_gray;
	cct->rgb = default_rgb;
	cct->bgr = default_bgr;
	cct->cmyk = default_cmyk;
	cct->lab = default_lab;
}