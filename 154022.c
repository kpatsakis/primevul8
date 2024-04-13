recalc_width(double old, double swidth, int cwidth,
	     double sxx, double syy, double sxy, int is_inclusive)
{
    double delta = swidth - (double)cwidth;
    double rat = sxy / sxx,
	coe = correlation_coefficient(sxx, syy, sxy), w, ww;
    if (old < 0.)
	old = 0.;
    if (fabs(coe) < 1e-5)
	return old;
    w = rat * old;
    ww = delta;
    if (w > 0.) {
	double wmin = 5e-3 * sqrt(syy * (1. - coe * coe));
	if (swidth < 0.2 && cwidth > 0 && is_inclusive) {
	    double coe1 = correlation_coefficient2(sxx, syy, sxy);
	    if (coe > 0.9 || coe1 > 0.9)
		return 0.;
	}
	if (wmin > 0.05)
	    wmin = 0.05;
	if (ww < 0.)
	    ww = 0.;
	ww += wmin;
    }
    else {
	double wmin = 5e-3 * sqrt(syy) * fabs(coe);
	if (rat > -0.001)
	    return old;
	if (wmin > 0.01)
	    wmin = 0.01;
	if (ww > 0.)
	    ww = 0.;
	ww -= wmin;
    }
    if (w > ww)
	return ww / rat;
    return old;
}