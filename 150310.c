int rtnl_nla_parse_ifla(struct nlattr **tb, const struct nlattr *head, int len)
{
	return nla_parse(tb, IFLA_MAX, head, len, ifla_policy);
}