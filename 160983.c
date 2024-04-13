xmlSaveFormatFile(const char *filename, xmlDocPtr cur, int format) {
    return ( xmlSaveFormatFileEnc( filename, cur, NULL, format ) );
}