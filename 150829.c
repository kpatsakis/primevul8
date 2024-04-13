xmlFreeNotationTable(xmlNotationTablePtr table) {
    xmlHashFree(table, (xmlHashDeallocator) xmlFreeNotation);
}