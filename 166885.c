proto_tree_add_bytes_item(proto_tree *tree, int hfindex, tvbuff_t *tvb,
			   const gint start, gint length, const guint encoding,
			   GByteArray *retval, gint *endoff, gint *err)
{
	field_info	  *new_fi;
	GByteArray	  *bytes = retval;
	GByteArray	  *created_bytes = NULL;
	gint		   saved_err = 0;
	guint32		   n = 0;
	header_field_info *hfinfo;
	gboolean	   generate = (bytes || tree) ? TRUE : FALSE;

	PROTO_REGISTRAR_GET_NTH(hfindex, hfinfo);

	DISSECTOR_ASSERT_HINT(hfinfo != NULL, "Not passed hfi!");

	DISSECTOR_ASSERT_HINT(validate_proto_tree_add_bytes_ftype(hfinfo->type),
		"Called proto_tree_add_bytes_item but not a bytes-based FT_XXX type");

	/* length has to be -1 or > 0 regardless of encoding */
	/* invalid FT_UINT_BYTES length is caught in get_uint_value() */
	if (length < -1 || length == 0) {
		REPORT_DISSECTOR_BUG("Invalid length %d passed to proto_tree_add_bytes_item for %s",
		    length, ftype_name(hfinfo->type));
	}

	if (encoding & ENC_STR_NUM) {
		REPORT_DISSECTOR_BUG("Decoding number strings for byte arrays is not supported");
	}

	if (generate && (encoding & ENC_STR_HEX)) {
		if (hfinfo->type == FT_UINT_BYTES) {
			/* can't decode FT_UINT_BYTES from strings */
			REPORT_DISSECTOR_BUG("proto_tree_add_bytes_item called for "
			    "FT_UINT_BYTES type, but as ENC_STR_HEX");
		}

		if (!bytes) {
			/* caller doesn't care about return value, but we need it to
			   call tvb_get_string_bytes() and set the tree later */
			bytes = created_bytes = g_byte_array_new();
		}

		/* bytes might be NULL after this, but can't add expert error until later */
		bytes = tvb_get_string_bytes(tvb, start, length, encoding, bytes, endoff);

		/* grab the errno now before it gets overwritten */
		saved_err = errno;
	}
	else if (generate) {
		tvb_ensure_bytes_exist(tvb, start, length);

		if (!bytes) {
			/* caller doesn't care about return value, but we need it to
			   call tvb_get_string_bytes() and set the tree later */
			bytes = created_bytes = g_byte_array_new();
		}

		if (hfinfo->type == FT_UINT_BYTES) {
			n = length; /* n is now the "header" length */
			length = get_uint_value(tree, tvb, start, n, encoding);
			/* length is now the value's length; only store the value in the array */
			g_byte_array_append(bytes, tvb_get_ptr(tvb, start + n, length), length);
		}
		else if (length > 0) {
			g_byte_array_append(bytes, tvb_get_ptr(tvb, start, length), length);
		}

		if (endoff)
		    *endoff = start + n + length;
	}

	if (err) *err = saved_err;

	CHECK_FOR_NULL_TREE_AND_FREE(tree,
		{
		    if (created_bytes)
			g_byte_array_free(created_bytes, TRUE);
		    created_bytes = NULL;
		    bytes = NULL;
		} );

	TRY_TO_FAKE_THIS_ITEM_OR_FREE(tree, hfinfo->id, hfinfo,
		{
		    if (created_bytes)
			g_byte_array_free(created_bytes, TRUE);
		    created_bytes = NULL;
		    bytes = NULL;
		} );

	/* n will be zero except when it's a FT_UINT_BYTES */
	new_fi = new_field_info(tree, hfinfo, tvb, start, n + length);

	if (encoding & ENC_STRING) {
		if (saved_err == ERANGE)
		    expert_add_info(NULL, tree, &ei_number_string_decoding_erange_error);
		else if (!bytes || saved_err != 0)
		    expert_add_info(NULL, tree, &ei_number_string_decoding_failed_error);

		if (bytes)
		    proto_tree_set_bytes_gbytearray(new_fi, bytes);
		else
		    proto_tree_set_bytes(new_fi, NULL, 0);

		if (created_bytes)
		    g_byte_array_free(created_bytes, TRUE);
	}
	else {
		/* n will be zero except when it's a FT_UINT_BYTES */
		proto_tree_set_bytes_tvb(new_fi, tvb, start + n, length);

		FI_SET_FLAG(new_fi,
			(encoding & ENC_LITTLE_ENDIAN) ? FI_LITTLE_ENDIAN : FI_BIG_ENDIAN);
	}

	return proto_tree_add_node(tree, new_fi);
}