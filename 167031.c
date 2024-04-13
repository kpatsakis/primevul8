proto_tree_prime_with_hfid(proto_tree *tree _U_, const gint hfid)
{
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfid, hfinfo);
	/* this field is referenced by a filter so increase the refcount.
	   also increase the refcount for the parent, i.e the protocol.
	*/
	hfinfo->ref_type = HF_REF_TYPE_DIRECT;
	/* only increase the refcount if there is a parent.
	   if this is a protocol and not a field then parent will be -1
	   and there is no parent to add any refcounting for.
	*/
	if (hfinfo->parent != -1) {
		header_field_info *parent_hfinfo;
		PROTO_REGISTRAR_GET_NTH(hfinfo->parent, parent_hfinfo);

		/* Mark parent as indirectly referenced unless it is already directly
		 * referenced, i.e. the user has specified the parent in a filter.
		 */
		if (parent_hfinfo->ref_type != HF_REF_TYPE_DIRECT)
			parent_hfinfo->ref_type = HF_REF_TYPE_INDIRECT;
	}
}