static int checkout_stream_close(git_writestream *s)
{
	struct checkout_stream *stream = (struct checkout_stream *)s;
	assert(stream && stream->open);

	stream->open = 0;
	return p_close(stream->fd);
}