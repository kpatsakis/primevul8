evbuffer_free(struct evbuffer *buffer)
{
	if (buffer->orig_buffer != NULL)
		free(buffer->orig_buffer);
	free(buffer);
}