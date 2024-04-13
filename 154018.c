correlation_coefficient2(double sxx, double syy, double sxy)
{
    double coe, tmp;
    tmp = (syy + sxx - 2 * sxy) * sxx;
    if (tmp < Tiny)
	tmp = Tiny;
    coe = (sxx - sxy) / sqrt(tmp);
    if (coe > 1.)
	return 1.;
    if (coe < -1.)
	return -1.;
    return coe;
}