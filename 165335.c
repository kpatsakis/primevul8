void zmq::stream_engine_t::set_handshake_timer ()
{
    zmq_assert (!has_handshake_timer);

    if (!options.raw_sock && options.handshake_ivl > 0) {
        add_timer (options.handshake_ivl, handshake_timer_id);
        has_handshake_timer = true;
    }
}