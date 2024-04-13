int bnxt_re_query_gid(struct ib_device *ibdev, u8 port_num,
		      int index, union ib_gid *gid)
{
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(ibdev, ibdev);
	int rc = 0;

	/* Ignore port_num */
	memset(gid, 0, sizeof(*gid));
	rc = bnxt_qplib_get_sgid(&rdev->qplib_res,
				 &rdev->qplib_res.sgid_tbl, index,
				 (struct bnxt_qplib_gid *)gid);
	return rc;
}