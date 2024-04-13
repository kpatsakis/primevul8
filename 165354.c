void zmq::stream_engine_t::plug (io_thread_t *io_thread_,
    session_base_t *session_)
{
    zmq_assert (!plugged);
    plugged = true;

    //  Connect to session object.
    zmq_assert (!session);
    zmq_assert (session_);
    session = session_;
    socket = session-> get_socket ();

    //  Connect to I/O threads poller object.
    io_object_t::plug (io_thread_);
    handle = add_fd (s);
    io_error = false;

    if (options.raw_sock) {
        // no handshaking for raw sock, instantiate raw encoder and decoders
        encoder = new (std::nothrow) raw_encoder_t (out_batch_size);
        alloc_assert (encoder);

        decoder = new (std::nothrow) raw_decoder_t (in_batch_size);
        alloc_assert (decoder);

        // disable handshaking for raw socket
        handshaking = false;

        next_msg = &stream_engine_t::pull_msg_from_session;
        process_msg = &stream_engine_t::push_msg_to_session;

        //  For raw sockets, send an initial 0-length message to the
        // application so that it knows a peer has connected.
        msg_t connector;
        connector.init();
        push_msg_to_session (&connector);
        connector.close();
        session->flush ();
    }
    else {
        // start optional timer, to prevent handshake hanging on no input
        set_handshake_timer ();

        //  Send the 'length' and 'flags' fields of the identity message.
        //  The 'length' field is encoded in the long format.
        outpos = greeting_send;
        outpos [outsize++] = 0xff;
        put_uint64 (&outpos [outsize], options.identity_size + 1);
        outsize += 8;
        outpos [outsize++] = 0x7f;
    }

    set_pollin (handle);
    set_pollout (handle);
    //  Flush all the data that may have been already received downstream.
    in_event ();
}