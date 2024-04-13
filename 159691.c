static bool match_disconn_id(const void *a, const void *b)
{
	const struct att_disconn *disconn = a;
	unsigned int id = PTR_TO_UINT(b);

	return disconn->id == id;
}