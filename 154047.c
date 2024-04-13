correlation_coefficient(double sxx, double syy, double sxy)
{
    double coe, tmp;
    tmp = sxx * syy;
    if (tmp < Tiny)
	tmp = Tiny;
    coe = sxy / sqrt(tmp);
    if (coe > 1.)
	return 1.;
    if (coe < -1.)
	return -1.;
    return coe;
}