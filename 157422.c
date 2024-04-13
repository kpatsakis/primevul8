static int bnxt_re_copy_inline_data(struct bnxt_re_dev *rdev,
				    const struct ib_send_wr *wr,
				    struct bnxt_qplib_swqe *wqe)
{
	/*  Copy the inline data to the data  field */
	u8 *in_data;
	u32 i, sge_len;
	void *sge_addr;

	in_data = wqe->inline_data;
	for (i = 0; i < wr->num_sge; i++) {
		sge_addr = (void *)(unsigned long)
				wr->sg_list[i].addr;
		sge_len = wr->sg_list[i].length;

		if ((sge_len + wqe->inline_len) >
		    BNXT_QPLIB_SWQE_MAX_INLINE_LENGTH) {
			dev_err(rdev_to_dev(rdev),
				"Inline data size requested > supported value");
			return -EINVAL;
		}
		sge_len = wr->sg_list[i].length;

		memcpy(in_data, sge_addr, sge_len);
		in_data += wr->sg_list[i].length;
		wqe->inline_len += wr->sg_list[i].length;
	}
	return wqe->inline_len;
}