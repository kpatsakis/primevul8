xmlGetDtdQAttrDesc(xmlDtdPtr dtd, const xmlChar *elem, const xmlChar *name,
	          const xmlChar *prefix) {
    xmlAttributeTablePtr table;

    if (dtd == NULL) return(NULL);
    if (dtd->attributes == NULL) return(NULL);
    table = (xmlAttributeTablePtr) dtd->attributes;

    return(xmlHashLookup3(table, name, prefix, elem));
}