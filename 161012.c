xmlBufDumpAttributeDecl(xmlBufPtr buf, xmlAttributePtr attr) {
    xmlBufferPtr buffer;

    buffer = xmlBufferCreate();
    if (buffer == NULL) {
        /*
         * TODO set the error in buf
         */
        return;
    }
    xmlDumpAttributeDecl(buffer, attr);
    xmlBufMergeBuffer(buf, buffer);
}