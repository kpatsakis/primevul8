acl_fetch_proto_http(struct proxy *px, struct session *l4, void *l7, unsigned int opt,
                     const struct arg *args, struct sample *smp)
{
	/* Note: hdr_idx.v cannot be NULL in this ACL because the ACL is tagged
	 * as a layer7 ACL, which involves automatic allocation of hdr_idx.
	 */

	CHECK_HTTP_MESSAGE_FIRST_PERM();

	smp->type = SMP_T_BOOL;
	smp->data.uint = 1;
	return 1;
}