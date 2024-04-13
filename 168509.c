static void debug_info(const char *what, struct attr_stack *elem)
{
	fprintf(stderr, "%s: %s\n", what, elem->origin ? elem->origin : "()");
}