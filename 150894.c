xmlGetID(xmlDocPtr doc, const xmlChar *ID) {
    xmlIDTablePtr table;
    xmlIDPtr id;

    if (doc == NULL) {
	return(NULL);
    }

    if (ID == NULL) {
	return(NULL);
    }

    table = (xmlIDTablePtr) doc->ids;
    if (table == NULL)
        return(NULL);

    id = xmlHashLookup(table, ID);
    if (id == NULL)
	return(NULL);
    if (id->attr == NULL) {
	/*
	 * We are operating on a stream, return a well known reference
	 * since the attribute node doesn't exist anymore
	 */
	return((xmlAttrPtr) doc);
    }
    return(id->attr);
}