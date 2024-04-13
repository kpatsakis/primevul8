xfs_agfl_read_verify(
	struct xfs_buf	*bp)
{
	struct xfs_mount *mp = bp->b_mount;
	xfs_failaddr_t	fa;

	/*
	 * There is no verification of non-crc AGFLs because mkfs does not
	 * initialise the AGFL to zero or NULL. Hence the only valid part of the
	 * AGFL is what the AGF says is active. We can't get to the AGF, so we
	 * can't verify just those entries are valid.
	 */
	if (!xfs_sb_version_hascrc(&mp->m_sb))
		return;

	if (!xfs_buf_verify_cksum(bp, XFS_AGFL_CRC_OFF))
		xfs_verifier_error(bp, -EFSBADCRC, __this_address);
	else {
		fa = xfs_agfl_verify(bp);
		if (fa)
			xfs_verifier_error(bp, -EFSCORRUPTED, fa);
	}
}