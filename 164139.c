htmlIsScriptAttribute(const xmlChar *name) {
    unsigned int i;

    if (name == NULL)
      return(0);
    /*
     * all script attributes start with 'on'
     */
    if ((name[0] != 'o') || (name[1] != 'n'))
      return(0);
    for (i = 0;
	 i < sizeof(htmlScriptAttributes)/sizeof(htmlScriptAttributes[0]);
	 i++) {
	if (xmlStrEqual(name, (const xmlChar *) htmlScriptAttributes[i]))
	    return(1);
    }
    return(0);
}