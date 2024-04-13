xfs_attr3_leaf_read_verify(
	struct xfs_buf		*bp)
{
	struct xfs_mount	*mp = bp->b_mount;
	xfs_failaddr_t		fa;

	if (xfs_sb_version_hascrc(&mp->m_sb) &&
	     !xfs_buf_verify_cksum(bp, XFS_ATTR3_LEAF_CRC_OFF))
		xfs_verifier_error(bp, -EFSBADCRC, __this_address);
	else {
		fa = xfs_attr3_leaf_verify(bp);
		if (fa)
			xfs_verifier_error(bp, -EFSCORRUPTED, fa);
	}
}