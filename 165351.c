void zmq::stream_engine_t::terminate ()
{
    unplug ();
    delete this;
}