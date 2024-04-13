htmlTagLookup(const xmlChar *tag) {
    unsigned int i;

    for (i = 0; i < (sizeof(html40ElementTable) /
                     sizeof(html40ElementTable[0]));i++) {
        if (!xmlStrcasecmp(tag, BAD_CAST html40ElementTable[i].name))
	    return((htmlElemDescPtr) &html40ElementTable[i]);
    }
    return(NULL);
}