xfs_agfl_needs_reset(
	struct xfs_mount	*mp,
	struct xfs_agf		*agf)
{
	uint32_t		f = be32_to_cpu(agf->agf_flfirst);
	uint32_t		l = be32_to_cpu(agf->agf_fllast);
	uint32_t		c = be32_to_cpu(agf->agf_flcount);
	int			agfl_size = xfs_agfl_size(mp);
	int			active;

	/* no agfl header on v4 supers */
	if (!xfs_sb_version_hascrc(&mp->m_sb))
		return false;

	/*
	 * The agf read verifier catches severe corruption of these fields.
	 * Repeat some sanity checks to cover a packed -> unpacked mismatch if
	 * the verifier allows it.
	 */
	if (f >= agfl_size || l >= agfl_size)
		return true;
	if (c > agfl_size)
		return true;

	/*
	 * Check consistency between the on-disk count and the active range. An
	 * agfl padding mismatch manifests as an inconsistent flcount.
	 */
	if (c && l >= f)
		active = l - f + 1;
	else if (c)
		active = agfl_size - f + l + 1;
	else
		active = 0;

	return active != c;
}