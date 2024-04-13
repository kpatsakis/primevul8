xmlDumpAttributeTable(xmlBufferPtr buf, xmlAttributeTablePtr table) {
    if ((buf == NULL) || (table == NULL))
        return;
    xmlHashScan(table, (xmlHashScanner) xmlDumpAttributeDeclScan, buf);
}