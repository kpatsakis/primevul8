int bnxt_re_query_pkey(struct ib_device *ibdev, u8 port_num,
		       u16 index, u16 *pkey)
{
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(ibdev, ibdev);

	/* Ignore port_num */

	memset(pkey, 0, sizeof(*pkey));
	return bnxt_qplib_get_pkey(&rdev->qplib_res,
				   &rdev->qplib_res.pkey_tbl, index, pkey);
}