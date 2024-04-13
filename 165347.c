void zmq::stream_engine_t::zap_msg_available ()
{
    zmq_assert (mechanism != NULL);

    const int rc = mechanism->zap_msg_available ();
    if (rc == -1) {
        error (protocol_error);
        return;
    }
    if (input_stopped)
        restart_input ();
    if (output_stopped)
        restart_output ();
}