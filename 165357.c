int zmq::stream_engine_t::process_handshake_command (msg_t *msg_)
{
    zmq_assert (mechanism != NULL);
    const int rc = mechanism->process_handshake_command (msg_);
    if (rc == 0) {
        if (mechanism->status () == mechanism_t::ready)
            mechanism_ready ();
        else
        if (mechanism->status () == mechanism_t::error) {
            errno = EPROTO;
            return -1;
        }
        if (output_stopped)
            restart_output ();
    }

    return rc;
}