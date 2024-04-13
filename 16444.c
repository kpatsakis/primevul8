
static __cold int io_rsrc_data_alloc(struct io_ring_ctx *ctx, rsrc_put_fn *do_put,
				     u64 __user *utags, unsigned nr,
				     struct io_rsrc_data **pdata)
{
	struct io_rsrc_data *data;
	int ret = -ENOMEM;
	unsigned i;

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;
	data->tags = (u64 **)io_alloc_page_table(nr * sizeof(data->tags[0][0]));
	if (!data->tags) {
		kfree(data);
		return -ENOMEM;
	}

	data->nr = nr;
	data->ctx = ctx;
	data->do_put = do_put;
	if (utags) {
		ret = -EFAULT;
		for (i = 0; i < nr; i++) {
			u64 *tag_slot = io_get_tag_slot(data, i);

			if (copy_from_user(tag_slot, &utags[i],
					   sizeof(*tag_slot)))
				goto fail;
		}
	}

	atomic_set(&data->refs, 1);
	init_completion(&data->done);
	*pdata = data;
	return 0;
fail:
	io_rsrc_data_free(data);
	return ret;