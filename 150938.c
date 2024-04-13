xmlGetDtdElementDesc2(xmlDtdPtr dtd, const xmlChar *name, int create) {
    xmlElementTablePtr table;
    xmlElementPtr cur;
    xmlChar *uqname = NULL, *prefix = NULL;

    if (dtd == NULL) return(NULL);
    if (dtd->elements == NULL) {
	xmlDictPtr dict = NULL;

	if (dtd->doc != NULL)
	    dict = dtd->doc->dict;

	if (!create)
	    return(NULL);
	/*
	 * Create the Element table if needed.
	 */
	table = (xmlElementTablePtr) dtd->elements;
	if (table == NULL) {
	    table = xmlHashCreateDict(0, dict);
	    dtd->elements = (void *) table;
	}
	if (table == NULL) {
	    xmlVErrMemory(NULL, "element table allocation failed");
	    return(NULL);
	}
    }
    table = (xmlElementTablePtr) dtd->elements;

    uqname = xmlSplitQName2(name, &prefix);
    if (uqname != NULL)
        name = uqname;
    cur = xmlHashLookup2(table, name, prefix);
    if ((cur == NULL) && (create)) {
	cur = (xmlElementPtr) xmlMalloc(sizeof(xmlElement));
	if (cur == NULL) {
	    xmlVErrMemory(NULL, "malloc failed");
	    return(NULL);
	}
	memset(cur, 0, sizeof(xmlElement));
	cur->type = XML_ELEMENT_DECL;

	/*
	 * fill the structure.
	 */
	cur->name = xmlStrdup(name);
	cur->prefix = xmlStrdup(prefix);
	cur->etype = XML_ELEMENT_TYPE_UNDEFINED;

	xmlHashAddEntry2(table, name, prefix, cur);
    }
    if (prefix != NULL) xmlFree(prefix);
    if (uqname != NULL) xmlFree(uqname);
    return(cur);
}