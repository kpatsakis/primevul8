hf_try_val_to_str_const(guint32 value, const header_field_info *hfinfo, const char *unknown_str)
{
	const char *str = hf_try_val_to_str(value, hfinfo);

	return (str) ? str : unknown_str;
}