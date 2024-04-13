xmlFreeAttributeTable(xmlAttributeTablePtr table) {
    xmlHashFree(table, (xmlHashDeallocator) xmlFreeAttribute);
}