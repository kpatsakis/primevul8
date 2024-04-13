clamp_indexed(const fz_colorspace *cs, const float *in, float *out)
{
	struct indexed *idx = cs->data;

	*out = fz_clamp(*in, 0, idx->high) / 255.0f; /* To do, avoid 255 divide */
}