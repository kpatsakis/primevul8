base_indexed(const fz_colorspace *cs)
{
	struct indexed *idx = cs->data;

	return idx->base;
}