static int ipv6_getsockopt_sticky(struct sock *sk, struct ipv6_txoptions *opt,
				  int optname, char __user *optval, int len)
{
	struct ipv6_opt_hdr *hdr;

	if (!opt)
		return 0;

	switch (optname) {
	case IPV6_HOPOPTS:
		hdr = opt->hopopt;
		break;
	case IPV6_RTHDRDSTOPTS:
		hdr = opt->dst0opt;
		break;
	case IPV6_RTHDR:
		hdr = (struct ipv6_opt_hdr *)opt->srcrt;
		break;
	case IPV6_DSTOPTS:
		hdr = opt->dst1opt;
		break;
	default:
		return -EINVAL;	/* should not happen */
	}

	if (!hdr)
		return 0;

	len = min_t(unsigned int, len, ipv6_optlen(hdr));
	if (copy_to_user(optval, hdr, len))
		return -EFAULT;
	return len;
}