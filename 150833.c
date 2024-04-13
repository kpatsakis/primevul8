xmlFreeIDTable(xmlIDTablePtr table) {
    xmlHashFree(table, (xmlHashDeallocator) xmlFreeID);
}