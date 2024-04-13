void zmq::stream_engine_t::restart_output ()
{
    if (unlikely (io_error))
        return;

    if (likely (output_stopped)) {
        set_pollout (handle);
        output_stopped = false;
    }

    //  Speculative write: The assumption is that at the moment new message
    //  was sent by the user the socket is probably available for writing.
    //  Thus we try to write the data to socket avoiding polling for POLLOUT.
    //  Consequently, the latency should be better in request/reply scenarios.
    out_event ();
}