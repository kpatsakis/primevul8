 */
static int __io_sqe_files_scm(struct io_ring_ctx *ctx, int nr, int offset)
{
	struct sock *sk = ctx->ring_sock->sk;
	struct scm_fp_list *fpl;
	struct sk_buff *skb;
	int i, nr_files;

	fpl = kzalloc(sizeof(*fpl), GFP_KERNEL);
	if (!fpl)
		return -ENOMEM;

	skb = alloc_skb(0, GFP_KERNEL);
	if (!skb) {
		kfree(fpl);
		return -ENOMEM;
	}

	skb->sk = sk;

	nr_files = 0;
	fpl->user = get_uid(current_user());
	for (i = 0; i < nr; i++) {
		struct file *file = io_file_from_index(ctx, i + offset);

		if (!file)
			continue;
		fpl->fp[nr_files] = get_file(file);
		unix_inflight(fpl->user, fpl->fp[nr_files]);
		nr_files++;
	}

	if (nr_files) {
		fpl->max = SCM_MAX_FD;
		fpl->count = nr_files;
		UNIXCB(skb).fp = fpl;
		skb->destructor = unix_destruct_scm;
		refcount_add(skb->truesize, &sk->sk_wmem_alloc);
		skb_queue_head(&sk->sk_receive_queue, skb);

		for (i = 0; i < nr; i++) {
			struct file *file = io_file_from_index(ctx, i + offset);

			if (file)
				fput(file);
		}
	} else {
		kfree_skb(skb);
		free_uid(fpl->user);
		kfree(fpl);
	}

	return 0;