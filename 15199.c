void Statement::Work_BeginEach(Baton* baton) {
    // Only create the Async object when we're actually going into
    // the event loop. This prevents dangling events.
    EachBaton* each_baton = static_cast<EachBaton*>(baton);
    each_baton->async = new Async(each_baton->stmt, reinterpret_cast<uv_async_cb>(AsyncEach));
    each_baton->async->item_cb.Reset(each_baton->callback.Value(), 1);
    each_baton->async->completed_cb.Reset(each_baton->completed.Value(), 1);

    STATEMENT_BEGIN(Each);
}