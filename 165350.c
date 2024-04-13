void zmq::stream_engine_t::restart_input ()
{
    zmq_assert (input_stopped);
    zmq_assert (session != NULL);
    zmq_assert (decoder != NULL);

    int rc = (this->*process_msg) (decoder->msg ());
    if (rc == -1) {
        if (errno == EAGAIN)
            session->flush ();
        else
            error (protocol_error);
        return;
    }

    while (insize > 0) {
        size_t processed = 0;
        rc = decoder->decode (inpos, insize, processed);
        zmq_assert (processed <= insize);
        inpos += processed;
        insize -= processed;
        if (rc == 0 || rc == -1)
            break;
        rc = (this->*process_msg) (decoder->msg ());
        if (rc == -1)
            break;
    }

    if (rc == -1 && errno == EAGAIN)
        session->flush ();
    else
    if (io_error)
        error (connection_error);
    else
    if (rc == -1)
        error (protocol_error);
    else {
        input_stopped = false;
        set_pollin (handle);
        session->flush ();

        //  Speculative read.
        in_event ();
    }
}