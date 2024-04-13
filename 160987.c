xmlBufDumpElementDecl(xmlBufPtr buf, xmlElementPtr elem) {
    xmlBufferPtr buffer;

    buffer = xmlBufferCreate();
    if (buffer == NULL) {
        /*
         * TODO set the error in buf
         */
        return;
    }
    xmlDumpElementDecl(buffer, elem);
    xmlBufMergeBuffer(buf, buffer);
}