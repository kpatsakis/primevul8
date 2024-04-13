static int validate_scan_freqs(struct nlattr *freqs)
{
	struct nlattr *attr1, *attr2;
	int n_channels = 0, tmp1, tmp2;

	nla_for_each_nested(attr1, freqs, tmp1) {
		n_channels++;
		/*
		 * Some hardware has a limited channel list for
		 * scanning, and it is pretty much nonsensical
		 * to scan for a channel twice, so disallow that
		 * and don't require drivers to check that the
		 * channel list they get isn't longer than what
		 * they can scan, as long as they can scan all
		 * the channels they registered at once.
		 */
		nla_for_each_nested(attr2, freqs, tmp2)
			if (attr1 != attr2 &&
			    nla_get_u32(attr1) == nla_get_u32(attr2))
				return 0;
	}

	return n_channels;
}