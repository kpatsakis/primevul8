hf_try_val64_to_str(guint64 value, const header_field_info *hfinfo)
{
	if (hfinfo->display & BASE_VAL64_STRING) {
		if (hfinfo->display & BASE_EXT_STRING)
			return try_val64_to_str_ext(value, (val64_string_ext *) hfinfo->strings);
		else
			return try_val64_to_str(value, (const val64_string *) hfinfo->strings);
	}

	if (hfinfo->display & BASE_RANGE_STRING)
		return try_rval64_to_str(value, (const range_string *) hfinfo->strings);

	if (hfinfo->display & BASE_UNIT_STRING)
		return unit_name_string_get_value64(value, (const struct unit_name_string*) hfinfo->strings);

	/* If this is reached somebody registered a 64-bit field with a 32-bit
	 * value-string, which isn't right. */
	REPORT_DISSECTOR_BUG("field %s is a 64-bit field with a 32-bit value_string",
	    hfinfo->abbrev);

	/* This is necessary to squelch MSVC errors; is there
	   any way to tell it that DISSECTOR_ASSERT_NOT_REACHED()
	   never returns? */
	return NULL;
}