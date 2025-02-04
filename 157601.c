static bool fib6_rule_suppress(struct fib_rule *rule, struct fib_lookup_arg *arg)
{
	struct fib6_result *res = arg->result;
	struct rt6_info *rt = res->rt6;
	struct net_device *dev = NULL;

	if (!rt)
		return false;

	if (rt->rt6i_idev)
		dev = rt->rt6i_idev->dev;

	/* do not accept result if the route does
	 * not meet the required prefix length
	 */
	if (rt->rt6i_dst.plen <= rule->suppress_prefixlen)
		goto suppress_route;

	/* do not accept result if the route uses a device
	 * belonging to a forbidden interface group
	 */
	if (rule->suppress_ifgroup != -1 && dev && dev->group == rule->suppress_ifgroup)
		goto suppress_route;

	return false;

suppress_route:
	if (!(arg->flags & FIB_LOOKUP_NOREF))
		ip6_rt_put(rt);
	return true;
}