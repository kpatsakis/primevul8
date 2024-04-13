proto_tree_add_time_item(proto_tree *tree, int hfindex, tvbuff_t *tvb,
			   const gint start, gint length, const guint encoding,
			   nstime_t *retval, gint *endoff, gint *err)
{
	field_info	  *new_fi;
	nstime_t	   time_stamp;
	gint		   saved_err = 0;
	header_field_info *hfinfo;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	DISSECTOR_ASSERT_FIELD_TYPE_IS_TIME(hfinfo);

	/* length has to be -1 or > 0 regardless of encoding */
	if (length < -1 || length == 0) {
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_time_item",
		    length);
	}

	time_stamp.secs  = 0;
	time_stamp.nsecs = 0;

	if (encoding & ENC_STR_TIME_MASK) {
		tvb_get_string_time(tvb, start, length, encoding, &time_stamp, endoff);
		/* grab the errno now before it gets overwritten */
		saved_err = errno;
	}
	else {
		const gboolean is_relative = (hfinfo->type == FT_RELATIVE_TIME) ? TRUE : FALSE;

		tvb_ensure_bytes_exist(tvb, start, length);
		get_time_value(tree, tvb, start, length, encoding, &time_stamp, is_relative);
		if (endoff) *endoff = length;
	}

	if (err) *err = saved_err;

	if (retval) {
		retval->secs  = time_stamp.secs;
		retval->nsecs = time_stamp.nsecs;
	}

	CHECK_FOR_NULL_TREE(tree);

	TRY_TO_FAKE_THIS_ITEM(tree, hfinfo->id, hfinfo);

	new_fi = new_field_info(tree, hfinfo, tvb, start, length);

	proto_tree_set_time(new_fi, &time_stamp);

	if (encoding & ENC_STRING) {
		if (saved_err == ERANGE)
		    expert_add_info(NULL, tree, &ei_number_string_decoding_erange_error);
		else if (saved_err == EDOM)
		    expert_add_info(NULL, tree, &ei_number_string_decoding_failed_error);
	}
	else {
		FI_SET_FLAG(new_fi,
			(encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN);
	}

	return proto_tree_add_node(tree, new_fi);
}