xmlGetDtdQElementDesc(xmlDtdPtr dtd, const xmlChar *name,
	              const xmlChar *prefix) {
    xmlElementTablePtr table;

    if (dtd == NULL) return(NULL);
    if (dtd->elements == NULL) return(NULL);
    table = (xmlElementTablePtr) dtd->elements;

    return(xmlHashLookup2(table, name, prefix));
}