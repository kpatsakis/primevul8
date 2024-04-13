proto_tree_set_appendix(proto_tree *tree, tvbuff_t *tvb, gint start,
			const gint length)
{
	field_info *fi;

	if (tree == NULL)
		return;

	fi = PTREE_FINFO(tree);
	if (fi == NULL)
		return;

	start += tvb_raw_offset(tvb);
	DISSECTOR_ASSERT(start >= 0);
	DISSECTOR_ASSERT(length >= 0);

	fi->appendix_start = start;
	fi->appendix_length = length;
}