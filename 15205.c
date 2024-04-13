void Statement::GetRow(Row* row, sqlite3_stmt* stmt) {
    int rows = sqlite3_column_count(stmt);

    for (int i = 0; i < rows; i++) {
        int type = sqlite3_column_type(stmt, i);
        const char* name = sqlite3_column_name(stmt, i);
        switch (type) {
            case SQLITE_INTEGER: {
                row->push_back(new Values::Integer(name, sqlite3_column_int64(stmt, i)));
            }   break;
            case SQLITE_FLOAT: {
                row->push_back(new Values::Float(name, sqlite3_column_double(stmt, i)));
            }   break;
            case SQLITE_TEXT: {
                const char* text = (const char*)sqlite3_column_text(stmt, i);
                int length = sqlite3_column_bytes(stmt, i);
                row->push_back(new Values::Text(name, length, text));
            } break;
            case SQLITE_BLOB: {
                const void* blob = sqlite3_column_blob(stmt, i);
                int length = sqlite3_column_bytes(stmt, i);
                row->push_back(new Values::Blob(name, length, blob));
            }   break;
            case SQLITE_NULL: {
                row->push_back(new Values::Null(name));
            }   break;
            default:
                assert(false);
        }
    }
}