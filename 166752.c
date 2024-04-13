validate_proto_tree_add_bytes_ftype(const enum ftenum type)
{
	return (type == FT_BYTES      ||
		type == FT_UINT_BYTES ||
		type == FT_OID        ||
		type == FT_REL_OID    ||
		type == FT_SYSTEM_ID  );
}