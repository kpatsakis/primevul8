xmlAddAttributeDecl(xmlValidCtxtPtr ctxt,
                    xmlDtdPtr dtd, const xmlChar *elem,
                    const xmlChar *name, const xmlChar *ns,
		    xmlAttributeType type, xmlAttributeDefault def,
		    const xmlChar *defaultValue, xmlEnumerationPtr tree) {
    xmlAttributePtr ret;
    xmlAttributeTablePtr table;
    xmlElementPtr elemDef;
    xmlDictPtr dict = NULL;

    if (dtd == NULL) {
	xmlFreeEnumeration(tree);
	return(NULL);
    }
    if (name == NULL) {
	xmlFreeEnumeration(tree);
	return(NULL);
    }
    if (elem == NULL) {
	xmlFreeEnumeration(tree);
	return(NULL);
    }
    if (dtd->doc != NULL)
	dict = dtd->doc->dict;

#ifdef LIBXML_VALID_ENABLED
    /*
     * Check the type and possibly the default value.
     */
    switch (type) {
        case XML_ATTRIBUTE_CDATA:
	    break;
        case XML_ATTRIBUTE_ID:
	    break;
        case XML_ATTRIBUTE_IDREF:
	    break;
        case XML_ATTRIBUTE_IDREFS:
	    break;
        case XML_ATTRIBUTE_ENTITY:
	    break;
        case XML_ATTRIBUTE_ENTITIES:
	    break;
        case XML_ATTRIBUTE_NMTOKEN:
	    break;
        case XML_ATTRIBUTE_NMTOKENS:
	    break;
        case XML_ATTRIBUTE_ENUMERATION:
	    break;
        case XML_ATTRIBUTE_NOTATION:
	    break;
	default:
	    xmlErrValid(ctxt, XML_ERR_INTERNAL_ERROR,
		    "Internal: ATTRIBUTE struct corrupted invalid type\n",
		    NULL);
	    xmlFreeEnumeration(tree);
	    return(NULL);
    }
    if ((defaultValue != NULL) &&
        (!xmlValidateAttributeValueInternal(dtd->doc, type, defaultValue))) {
	xmlErrValidNode(ctxt, (xmlNodePtr) dtd, XML_DTD_ATTRIBUTE_DEFAULT,
	                "Attribute %s of %s: invalid default value\n",
	                elem, name, defaultValue);
	defaultValue = NULL;
	if (ctxt != NULL)
	    ctxt->valid = 0;
    }
#endif /* LIBXML_VALID_ENABLED */

    /*
     * Check first that an attribute defined in the external subset wasn't
     * already defined in the internal subset
     */
    if ((dtd->doc != NULL) && (dtd->doc->extSubset == dtd) &&
	(dtd->doc->intSubset != NULL) &&
	(dtd->doc->intSubset->attributes != NULL)) {
        ret = xmlHashLookup3(dtd->doc->intSubset->attributes, name, ns, elem);
	if (ret != NULL) {
	    xmlFreeEnumeration(tree);
	    return(NULL);
	}
    }

    /*
     * Create the Attribute table if needed.
     */
    table = (xmlAttributeTablePtr) dtd->attributes;
    if (table == NULL) {
        table = xmlHashCreateDict(0, dict);
	dtd->attributes = (void *) table;
    }
    if (table == NULL) {
	xmlVErrMemory(ctxt,
            "xmlAddAttributeDecl: Table creation failed!\n");
	xmlFreeEnumeration(tree);
        return(NULL);
    }


    ret = (xmlAttributePtr) xmlMalloc(sizeof(xmlAttribute));
    if (ret == NULL) {
	xmlVErrMemory(ctxt, "malloc failed");
	xmlFreeEnumeration(tree);
	return(NULL);
    }
    memset(ret, 0, sizeof(xmlAttribute));
    ret->type = XML_ATTRIBUTE_DECL;

    /*
     * fill the structure.
     */
    ret->atype = type;
    /*
     * doc must be set before possible error causes call
     * to xmlFreeAttribute (because it's used to check on
     * dict use)
     */
    ret->doc = dtd->doc;
    if (dict) {
	ret->name = xmlDictLookup(dict, name, -1);
	ret->prefix = xmlDictLookup(dict, ns, -1);
	ret->elem = xmlDictLookup(dict, elem, -1);
    } else {
	ret->name = xmlStrdup(name);
	ret->prefix = xmlStrdup(ns);
	ret->elem = xmlStrdup(elem);
    }
    ret->def = def;
    ret->tree = tree;
    if (defaultValue != NULL) {
        if (dict)
	    ret->defaultValue = xmlDictLookup(dict, defaultValue, -1);
	else
	    ret->defaultValue = xmlStrdup(defaultValue);
    }

    /*
     * Validity Check:
     * Search the DTD for previous declarations of the ATTLIST
     */
    if (xmlHashAddEntry3(table, ret->name, ret->prefix, ret->elem, ret) < 0) {
#ifdef LIBXML_VALID_ENABLED
	/*
	 * The attribute is already defined in this DTD.
	 */
	xmlErrValidWarning(ctxt, (xmlNodePtr) dtd, XML_DTD_ATTRIBUTE_REDEFINED,
		 "Attribute %s of element %s: already defined\n",
		 name, elem, NULL);
#endif /* LIBXML_VALID_ENABLED */
	xmlFreeAttribute(ret);
	return(NULL);
    }

    /*
     * Validity Check:
     * Multiple ID per element
     */
    elemDef = xmlGetDtdElementDesc2(dtd, elem, 1);
    if (elemDef != NULL) {

#ifdef LIBXML_VALID_ENABLED
        if ((type == XML_ATTRIBUTE_ID) &&
	    (xmlScanIDAttributeDecl(NULL, elemDef, 1) != 0)) {
	    xmlErrValidNode(ctxt, (xmlNodePtr) dtd, XML_DTD_MULTIPLE_ID,
	   "Element %s has too may ID attributes defined : %s\n",
		   elem, name, NULL);
	    if (ctxt != NULL)
		ctxt->valid = 0;
	}
#endif /* LIBXML_VALID_ENABLED */

	/*
	 * Insert namespace default def first they need to be
	 * processed first.
	 */
	if ((xmlStrEqual(ret->name, BAD_CAST "xmlns")) ||
	    ((ret->prefix != NULL &&
	     (xmlStrEqual(ret->prefix, BAD_CAST "xmlns"))))) {
	    ret->nexth = elemDef->attributes;
	    elemDef->attributes = ret;
	} else {
	    xmlAttributePtr tmp = elemDef->attributes;

	    while ((tmp != NULL) &&
		   ((xmlStrEqual(tmp->name, BAD_CAST "xmlns")) ||
		    ((ret->prefix != NULL &&
		     (xmlStrEqual(ret->prefix, BAD_CAST "xmlns")))))) {
		if (tmp->nexth == NULL)
		    break;
		tmp = tmp->nexth;
	    }
	    if (tmp != NULL) {
		ret->nexth = tmp->nexth;
	        tmp->nexth = ret;
	    } else {
		ret->nexth = elemDef->attributes;
		elemDef->attributes = ret;
	    }
	}
    }

    /*
     * Link it to the DTD
     */
    ret->parent = dtd;
    if (dtd->last == NULL) {
	dtd->children = dtd->last = (xmlNodePtr) ret;
    } else {
        dtd->last->next = (xmlNodePtr) ret;
	ret->prev = dtd->last;
	dtd->last = (xmlNodePtr) ret;
    }
    return(ret);
}