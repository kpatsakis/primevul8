js_grisu2(double v, char *buffer, int *K)
{
	int length, mk;
	diy_fp_t w_m, w_p, c_mk, Wp, Wm, delta;
	int q = 64, alpha = -59, gamma = -56;
	normalized_boundaries(v, &w_m, &w_p);
	mk = k_comp(w_p.e + q, alpha, gamma);
	c_mk = cached_power(mk);
	Wp = multiply(w_p, c_mk);
	Wm = multiply(w_m, c_mk);
	Wm.f++; Wp.f--;
	delta = minus(Wp, Wm);
	*K = -mk;
	digit_gen(Wp, delta, buffer, &length, K);
	return length;
}