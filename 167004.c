hf_try_val64_to_str_const(guint64 value, const header_field_info *hfinfo, const char *unknown_str)
{
	const char *str = hf_try_val64_to_str(value, hfinfo);

	return (str) ? str : unknown_str;
}