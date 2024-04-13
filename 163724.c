acl_fetch_http_auth_grp(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                        const struct arg *args, struct sample *smp)
{

	if (!args || args->type != ARGT_USR)
		return 0;

	CHECK_HTTP_MESSAGE_FIRST();

	if (!get_http_auth(l4))
		return 0;

	/* acl_match_auth() will need several information at once */
	smp->ctx.a[0] = args->data.usr;      /* user list */
	smp->ctx.a[1] = l4->txn.auth.user;   /* user name */
	smp->ctx.a[2] = l4->txn.auth.pass;   /* password */

	/* if the user does not belong to the userlist or has a wrong password,
	 * report that it unconditionally does not match. Otherwise we return
	 * a non-zero integer which will be ignored anyway since all the params
	 * that acl_match_auth() will use are in test->ctx.a[0,1,2].
	 */
	smp->type = SMP_T_BOOL;
	smp->data.uint = check_user(args->data.usr, 0, l4->txn.auth.user, l4->txn.auth.pass);
	if (smp->data.uint)
		smp->type = SMP_T_UINT;

	return 1;
}