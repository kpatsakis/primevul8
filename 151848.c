bool red_stream_set_no_delay(RedStream *stream, bool no_delay)
{
    return red_socket_set_no_delay(stream->socket, no_delay);
}