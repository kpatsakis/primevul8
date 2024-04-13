xmlDumpElementTable(xmlBufferPtr buf, xmlElementTablePtr table) {
    if ((buf == NULL) || (table == NULL))
        return;
    xmlHashScan(table, (xmlHashScanner) xmlDumpElementDeclScan, buf);
}