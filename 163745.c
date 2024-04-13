static int acl_match_meth(struct sample *smp, struct acl_pattern *pattern)
{
	int icase;


	if (smp->type == SMP_T_UINT) {
		/* well-known method */
		if (smp->data.uint == pattern->val.i)
			return ACL_PAT_PASS;
		return ACL_PAT_FAIL;
	}

	/* Uncommon method, only HTTP_METH_OTHER is accepted now */
	if (pattern->val.i != HTTP_METH_OTHER)
		return ACL_PAT_FAIL;

	/* Other method, we must compare the strings */
	if (pattern->len != smp->data.str.len)
		return ACL_PAT_FAIL;

	icase = pattern->flags & ACL_PAT_F_IGNORE_CASE;
	if ((icase && strncasecmp(pattern->ptr.str, smp->data.str.str, smp->data.str.len) != 0) ||
	    (!icase && strncmp(pattern->ptr.str, smp->data.str.str, smp->data.str.len) != 0))
		return ACL_PAT_FAIL;
	return ACL_PAT_PASS;
}