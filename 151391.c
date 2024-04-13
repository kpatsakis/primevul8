evbuffer_new(void)
{
	struct evbuffer *buffer;
	
	buffer = calloc(1, sizeof(struct evbuffer));

	return (buffer);
}