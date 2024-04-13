zmq::stream_engine_t::~stream_engine_t ()
{
    zmq_assert (!plugged);

    if (s != retired_fd) {
#ifdef ZMQ_HAVE_WINDOWS
        int rc = closesocket (s);
        wsa_assert (rc != SOCKET_ERROR);
#else
        int rc = close (s);
        errno_assert (rc == 0);
#endif
        s = retired_fd;
    }

    int rc = tx_msg.close ();
    errno_assert (rc == 0);

    //  Drop reference to metadata and destroy it if we are
    //  the only user.
    if (metadata != NULL)
        if (metadata->drop_ref ())
            delete metadata;

    delete encoder;
    delete decoder;
    delete mechanism;
}