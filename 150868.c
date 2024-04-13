xmlDumpNotationTable(xmlBufferPtr buf, xmlNotationTablePtr table) {
    if ((buf == NULL) || (table == NULL))
        return;
    xmlHashScan(table, (xmlHashScanner) xmlDumpNotationDeclScan, buf);
}