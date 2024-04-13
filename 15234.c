Napi::Value Statement::RowToJS(Napi::Env env, Row* row) {
    Napi::EscapableHandleScope scope(env);

    Napi::Object result = Napi::Object::New(env);

    Row::const_iterator it = row->begin();
    Row::const_iterator end = row->end();
    for (int i = 0; it < end; ++it, i++) {
        Values::Field* field = *it;

        Napi::Value value;

        switch (field->type) {
            case SQLITE_INTEGER: {
                value = Napi::Number::New(env, ((Values::Integer*)field)->value);
            } break;
            case SQLITE_FLOAT: {
                value = Napi::Number::New(env, ((Values::Float*)field)->value);
            } break;
            case SQLITE_TEXT: {
                value = Napi::String::New(env, ((Values::Text*)field)->value.c_str(), ((Values::Text*)field)->value.size());
            } break;
            case SQLITE_BLOB: {
                value = Napi::Buffer<char>::Copy(env, ((Values::Blob*)field)->value, ((Values::Blob*)field)->length);
            } break;
            case SQLITE_NULL: {
                value = env.Null();
            } break;
        }

        (result).Set(Napi::String::New(env, field->name.c_str()), value);

        DELETE_FIELD(field);
    }

    return scope.Escape(result);
}