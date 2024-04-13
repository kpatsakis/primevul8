xmlGetRefs(xmlDocPtr doc, const xmlChar *ID) {
    xmlRefTablePtr table;

    if (doc == NULL) {
        return(NULL);
    }

    if (ID == NULL) {
        return(NULL);
    }

    table = (xmlRefTablePtr) doc->refs;
    if (table == NULL)
        return(NULL);

    return (xmlHashLookup(table, ID));
}