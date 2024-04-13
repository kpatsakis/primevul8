xmlBufDumpNotationTable(xmlBufPtr buf, xmlNotationTablePtr table) {
    xmlBufferPtr buffer;

    buffer = xmlBufferCreate();
    if (buffer == NULL) {
        /*
         * TODO set the error in buf
         */
        return;
    }
    xmlDumpNotationTable(buffer, table);
    xmlBufMergeBuffer(buf, buffer);
}