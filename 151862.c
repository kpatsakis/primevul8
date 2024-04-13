int red_stream_get_no_delay(RedStream *stream)
{
    return red_socket_get_no_delay(stream->socket);
}