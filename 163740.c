smp_fetch_url_param_val(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                     const struct arg *args, struct sample *smp)
{
	int ret = smp_fetch_url_param(px, l4, l7, opt, args, smp);

	if (ret > 0) {
		smp->type = SMP_T_UINT;
		smp->data.uint = strl2ic(smp->data.str.str, smp->data.str.len);
	}

	return ret;
}