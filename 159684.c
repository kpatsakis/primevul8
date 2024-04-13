static bool match_notify_id(const void *a, const void *b)
{
	const struct att_notify *notify = a;
	unsigned int id = PTR_TO_UINT(b);

	return notify->id == id;
}