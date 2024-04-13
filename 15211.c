void Statement::AsyncEach(uv_async_t* handle) {
    Async* async = static_cast<Async*>(handle->data);

    Napi::Env env = async->stmt->Env();
    Napi::HandleScope scope(env);

    while (true) {
        // Get the contents out of the data cache for us to process in the JS callback.
        Rows rows;
        NODE_SQLITE3_MUTEX_LOCK(&async->mutex)
        rows.swap(async->data);
        NODE_SQLITE3_MUTEX_UNLOCK(&async->mutex)

        if (rows.empty()) {
            break;
        }

        Napi::Function cb = async->item_cb.Value();
        if (!cb.IsUndefined() && cb.IsFunction()) {
            Napi::Value argv[2];
            argv[0] = env.Null();

            Rows::const_iterator it = rows.begin();
            Rows::const_iterator end = rows.end();
            for (int i = 0; it < end; ++it, i++) {
                std::unique_ptr<Row> row(*it);
                argv[1] = RowToJS(env,row.get());
                async->retrieved++;
                TRY_CATCH_CALL(async->stmt->Value(), cb, 2, argv);
            }
        }
    }

    Napi::Function cb = async->completed_cb.Value();
    if (async->completed) {
        if (!cb.IsEmpty() &&
                cb.IsFunction()) {
            Napi::Value argv[] = {
                env.Null(),
                Napi::Number::New(env, async->retrieved)
            };
            TRY_CATCH_CALL(async->stmt->Value(), cb, 2, argv);
        }
        uv_close(reinterpret_cast<uv_handle_t*>(handle), CloseCallback);
    }
}