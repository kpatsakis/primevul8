xmlGetDtdAttrDesc(xmlDtdPtr dtd, const xmlChar *elem, const xmlChar *name) {
    xmlAttributeTablePtr table;
    xmlAttributePtr cur;
    xmlChar *uqname = NULL, *prefix = NULL;

    if (dtd == NULL) return(NULL);
    if (dtd->attributes == NULL) return(NULL);

    table = (xmlAttributeTablePtr) dtd->attributes;
    if (table == NULL)
	return(NULL);

    uqname = xmlSplitQName2(name, &prefix);

    if (uqname != NULL) {
	cur = xmlHashLookup3(table, uqname, prefix, elem);
	if (prefix != NULL) xmlFree(prefix);
	if (uqname != NULL) xmlFree(uqname);
    } else
	cur = xmlHashLookup3(table, name, NULL, elem);
    return(cur);
}