sg_link_reserve(Sg_fd * sfp, Sg_request * srp, int size)
{
	Sg_scatter_hold *req_schp = &srp->data;
	Sg_scatter_hold *rsv_schp = &sfp->reserve;
	int k, num, rem;

	srp->res_used = 1;
	SCSI_LOG_TIMEOUT(4, sg_printk(KERN_INFO, sfp->parentdp,
			 "sg_link_reserve: size=%d\n", size));
	rem = size;

	num = 1 << (PAGE_SHIFT + rsv_schp->page_order);
	for (k = 0; k < rsv_schp->k_use_sg; k++) {
		if (rem <= num) {
			req_schp->k_use_sg = k + 1;
			req_schp->sglist_len = rsv_schp->sglist_len;
			req_schp->pages = rsv_schp->pages;

			req_schp->bufflen = size;
			req_schp->page_order = rsv_schp->page_order;
			break;
		} else
			rem -= num;
	}

	if (k >= rsv_schp->k_use_sg)
		SCSI_LOG_TIMEOUT(1, sg_printk(KERN_INFO, sfp->parentdp,
				 "sg_link_reserve: BAD size\n"));
}