xmlGetDtdElementDesc(xmlDtdPtr dtd, const xmlChar *name) {
    xmlElementTablePtr table;
    xmlElementPtr cur;
    xmlChar *uqname = NULL, *prefix = NULL;

    if ((dtd == NULL) || (name == NULL)) return(NULL);
    if (dtd->elements == NULL)
	return(NULL);
    table = (xmlElementTablePtr) dtd->elements;

    uqname = xmlSplitQName2(name, &prefix);
    if (uqname != NULL)
        name = uqname;
    cur = xmlHashLookup2(table, name, prefix);
    if (prefix != NULL) xmlFree(prefix);
    if (uqname != NULL) xmlFree(uqname);
    return(cur);
}