static void proc_self_put_link(struct dentry *dentry, struct nameidata *nd,
				void *cookie)
{
	char *s = nd_get_link(nd);
	if (!IS_ERR(s))
		__putname(s);
}