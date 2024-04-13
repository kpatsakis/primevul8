xmlGetDtdNotationDesc(xmlDtdPtr dtd, const xmlChar *name) {
    xmlNotationTablePtr table;

    if (dtd == NULL) return(NULL);
    if (dtd->notations == NULL) return(NULL);
    table = (xmlNotationTablePtr) dtd->notations;

    return(xmlHashLookup(table, name));
}