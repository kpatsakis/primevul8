xmlDocDump(FILE *f, xmlDocPtr cur) {
    return(xmlDocFormatDump (f, cur, 0));
}