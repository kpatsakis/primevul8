xmlBufDumpEntityDecl(xmlBufPtr buf, xmlEntityPtr ent) {
    xmlBufferPtr buffer;

    buffer = xmlBufferCreate();
    if (buffer == NULL) {
        /*
         * TODO set the error in buf
         */
        return;
    }
    xmlDumpEntityDecl(buffer, ent);
    xmlBufMergeBuffer(buf, buffer);
}