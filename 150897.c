xmlFreeElementTable(xmlElementTablePtr table) {
    xmlHashFree(table, (xmlHashDeallocator) xmlFreeElement);
}