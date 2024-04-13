new_field_info(proto_tree *tree, header_field_info *hfinfo, tvbuff_t *tvb,
	       const gint start, const gint item_length)
{
	field_info *fi;

	FIELD_INFO_NEW(PNODE_POOL(tree), fi);

	fi->hfinfo     = hfinfo;
	fi->start      = start;
	fi->start     += (tvb)?tvb_raw_offset(tvb):0;
	fi->length     = item_length;
	fi->tree_type  = -1;
	fi->flags      = 0;
	if (!PTREE_DATA(tree)->visible)
		FI_SET_FLAG(fi, FI_HIDDEN);
	fvalue_init(&fi->value, fi->hfinfo->type);
	fi->rep        = NULL;

	/* add the data source tvbuff */
	fi->ds_tvb = tvb ? tvb_get_ds_tvb(tvb) : NULL;

	fi->appendix_start  = 0;
	fi->appendix_length = 0;

	return fi;
}