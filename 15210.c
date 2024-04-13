void Statement::CleanQueue() {
    Napi::Env env = this->Env();
    Napi::HandleScope scope(env);

    if (prepared && !queue.empty()) {
        // This statement has already been prepared and is now finalized.
        // Fire error for all remaining items in the queue.
        EXCEPTION(Napi::String::New(env, "Statement is already finalized"), SQLITE_MISUSE, exception);
        Napi::Value argv[] = { exception };
        bool called = false;

        // Clear out the queue so that this object can get GC'ed.
        while (!queue.empty()) {
            std::unique_ptr<Call> call(queue.front());
            queue.pop();

            std::unique_ptr<Baton> baton(call->baton);
            Napi::Function cb = baton->callback.Value();

            if (prepared && !cb.IsEmpty() &&
                cb.IsFunction()) {
                TRY_CATCH_CALL(Value(), cb, 1, argv);
                called = true;
            }
        }

        // When we couldn't call a callback function, emit an error on the
        // Statement object.
        if (!called) {
            Napi::Value info[] = { Napi::String::New(env, "error"), exception };
            EMIT_EVENT(Value(), 2, info);
        }
    }
    else while (!queue.empty()) {
        // Just delete all items in the queue; we already fired an event when
        // preparing the statement failed.
        std::unique_ptr<Call> call(queue.front());
        queue.pop();
        // We don't call the actual callback, so we have to make sure that
        // the baton gets destroyed.
        delete call->baton;
    }
}