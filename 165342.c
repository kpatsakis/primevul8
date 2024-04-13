void zmq::stream_engine_t::unplug ()
{
    zmq_assert (plugged);
    plugged = false;

    //  Cancel all timers.
    if (has_handshake_timer) {
        cancel_timer (handshake_timer_id);
        has_handshake_timer = false;
    }

    //  Cancel all fd subscriptions.
    if (!io_error)
        rm_fd (handle);

    //  Disconnect from I/O threads poller object.
    io_object_t::unplug ();

    session = NULL;
}