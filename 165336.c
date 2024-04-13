void zmq::stream_engine_t::error (error_reason_t reason)
{
    if (options.raw_sock) {
        //  For raw sockets, send a final 0-length message to the application
        //  so that it knows the peer has been disconnected.
        msg_t terminator;
        terminator.init();
        (this->*process_msg) (&terminator);
        terminator.close();
    }
    zmq_assert (session);
    socket->event_disconnected (endpoint, s);
    session->flush ();
    session->engine_error (reason);
    unplug ();
    delete this;
}