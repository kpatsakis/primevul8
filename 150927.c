xmlValidateAttributeDecl(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                         xmlAttributePtr attr) {
    int ret = 1;
    int val;
    CHECK_DTD;
    if(attr == NULL) return(1);

    /* Attribute Default Legal */
    /* Enumeration */
    if (attr->defaultValue != NULL) {
	val = xmlValidateAttributeValueInternal(doc, attr->atype,
	                                        attr->defaultValue);
	if (val == 0) {
	    xmlErrValidNode(ctxt, (xmlNodePtr) attr, XML_DTD_ATTRIBUTE_DEFAULT,
	       "Syntax of default value for attribute %s of %s is not valid\n",
	           attr->name, attr->elem, NULL);
	}
        ret &= val;
    }

    /* ID Attribute Default */
    if ((attr->atype == XML_ATTRIBUTE_ID)&&
        (attr->def != XML_ATTRIBUTE_IMPLIED) &&
	(attr->def != XML_ATTRIBUTE_REQUIRED)) {
	xmlErrValidNode(ctxt, (xmlNodePtr) attr, XML_DTD_ID_FIXED,
          "ID attribute %s of %s is not valid must be #IMPLIED or #REQUIRED\n",
	       attr->name, attr->elem, NULL);
	ret = 0;
    }

    /* One ID per Element Type */
    if (attr->atype == XML_ATTRIBUTE_ID) {
        int nbId;

	/* the trick is that we parse DtD as their own internal subset */
        xmlElementPtr elem = xmlGetDtdElementDesc(doc->intSubset,
	                                          attr->elem);
	if (elem != NULL) {
	    nbId = xmlScanIDAttributeDecl(NULL, elem, 0);
	} else {
	    xmlAttributeTablePtr table;

	    /*
	     * The attribute may be declared in the internal subset and the
	     * element in the external subset.
	     */
	    nbId = 0;
	    if (doc->intSubset != NULL) {
		table = (xmlAttributeTablePtr) doc->intSubset->attributes;
		xmlHashScan3(table, NULL, NULL, attr->elem, (xmlHashScanner)
			     xmlValidateAttributeIdCallback, &nbId);
	    }
	}
	if (nbId > 1) {

	    xmlErrValidNodeNr(ctxt, (xmlNodePtr) attr, XML_DTD_ID_SUBSET,
       "Element %s has %d ID attribute defined in the internal subset : %s\n",
		   attr->elem, nbId, attr->name);
	} else if (doc->extSubset != NULL) {
	    int extId = 0;
	    elem = xmlGetDtdElementDesc(doc->extSubset, attr->elem);
	    if (elem != NULL) {
		extId = xmlScanIDAttributeDecl(NULL, elem, 0);
	    }
	    if (extId > 1) {
		xmlErrValidNodeNr(ctxt, (xmlNodePtr) attr, XML_DTD_ID_SUBSET,
       "Element %s has %d ID attribute defined in the external subset : %s\n",
		       attr->elem, extId, attr->name);
	    } else if (extId + nbId > 1) {
		xmlErrValidNode(ctxt, (xmlNodePtr) attr, XML_DTD_ID_SUBSET,
"Element %s has ID attributes defined in the internal and external subset : %s\n",
		       attr->elem, attr->name, NULL);
	    }
	}
    }

    /* Validity Constraint: Enumeration */
    if ((attr->defaultValue != NULL) && (attr->tree != NULL)) {
        xmlEnumerationPtr tree = attr->tree;
	while (tree != NULL) {
	    if (xmlStrEqual(tree->name, attr->defaultValue)) break;
	    tree = tree->next;
	}
	if (tree == NULL) {
	    xmlErrValidNode(ctxt, (xmlNodePtr) attr, XML_DTD_ATTRIBUTE_VALUE,
"Default value \"%s\" for attribute %s of %s is not among the enumerated set\n",
		   attr->defaultValue, attr->name, attr->elem);
	    ret = 0;
	}
    }

    return(ret);
}