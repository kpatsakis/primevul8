xmlFreeRefTable(xmlRefTablePtr table) {
    xmlHashFree(table, (xmlHashDeallocator) xmlFreeRefList);
}