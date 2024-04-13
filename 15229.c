bool Statement::Bind(const Parameters & parameters) {
    if (parameters.size() == 0) {
        return true;
    }

    sqlite3_reset(_handle);
    sqlite3_clear_bindings(_handle);

    Parameters::const_iterator it = parameters.begin();
    Parameters::const_iterator end = parameters.end();

    for (; it < end; ++it) {
        Values::Field* field = *it;

        if (field != NULL) {
            unsigned int pos;
            if (field->index > 0) {
                pos = field->index;
            }
            else {
                pos = sqlite3_bind_parameter_index(_handle, field->name.c_str());
            }

            switch (field->type) {
                case SQLITE_INTEGER: {
                    status = sqlite3_bind_int(_handle, pos,
                        ((Values::Integer*)field)->value);
                } break;
                case SQLITE_FLOAT: {
                    status = sqlite3_bind_double(_handle, pos,
                        ((Values::Float*)field)->value);
                } break;
                case SQLITE_TEXT: {
                    status = sqlite3_bind_text(_handle, pos,
                        ((Values::Text*)field)->value.c_str(),
                        ((Values::Text*)field)->value.size(), SQLITE_TRANSIENT);
                } break;
                case SQLITE_BLOB: {
                    status = sqlite3_bind_blob(_handle, pos,
                        ((Values::Blob*)field)->value,
                        ((Values::Blob*)field)->length, SQLITE_TRANSIENT);
                } break;
                case SQLITE_NULL: {
                    status = sqlite3_bind_null(_handle, pos);
                } break;
            }

            if (status != SQLITE_OK) {
                message = std::string(sqlite3_errmsg(db->_handle));
                return false;
            }
        }
    }

    return true;
}