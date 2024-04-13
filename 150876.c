xmlValidateOneAttribute(xmlValidCtxtPtr ctxt, xmlDocPtr doc,
                        xmlNodePtr elem, xmlAttrPtr attr, const xmlChar *value)
{
    xmlAttributePtr attrDecl =  NULL;
    int val;
    int ret = 1;

    CHECK_DTD;
    if ((elem == NULL) || (elem->name == NULL)) return(0);
    if ((attr == NULL) || (attr->name == NULL)) return(0);

    if ((elem->ns != NULL) && (elem->ns->prefix != NULL)) {
	xmlChar fn[50];
	xmlChar *fullname;

	fullname = xmlBuildQName(elem->name, elem->ns->prefix, fn, 50);
	if (fullname == NULL)
	    return(0);
	if (attr->ns != NULL) {
	    attrDecl = xmlGetDtdQAttrDesc(doc->intSubset, fullname,
		                          attr->name, attr->ns->prefix);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdQAttrDesc(doc->extSubset, fullname,
					      attr->name, attr->ns->prefix);
	} else {
	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset, fullname, attr->name);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset,
					     fullname, attr->name);
	}
	if ((fullname != fn) && (fullname != elem->name))
	    xmlFree(fullname);
    }
    if (attrDecl == NULL) {
	if (attr->ns != NULL) {
	    attrDecl = xmlGetDtdQAttrDesc(doc->intSubset, elem->name,
		                          attr->name, attr->ns->prefix);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdQAttrDesc(doc->extSubset, elem->name,
					      attr->name, attr->ns->prefix);
	} else {
	    attrDecl = xmlGetDtdAttrDesc(doc->intSubset,
		                         elem->name, attr->name);
	    if ((attrDecl == NULL) && (doc->extSubset != NULL))
		attrDecl = xmlGetDtdAttrDesc(doc->extSubset,
					     elem->name, attr->name);
	}
    }


    /* Validity Constraint: Attribute Value Type */
    if (attrDecl == NULL) {
	xmlErrValidNode(ctxt, elem, XML_DTD_UNKNOWN_ATTRIBUTE,
	       "No declaration for attribute %s of element %s\n",
	       attr->name, elem->name, NULL);
	return(0);
    }
    attr->atype = attrDecl->atype;

    val = xmlValidateAttributeValueInternal(doc, attrDecl->atype, value);
    if (val == 0) {
	    xmlErrValidNode(ctxt, elem, XML_DTD_ATTRIBUTE_VALUE,
	   "Syntax of value for attribute %s of %s is not valid\n",
	       attr->name, elem->name, NULL);
        ret = 0;
    }

    /* Validity constraint: Fixed Attribute Default */
    if (attrDecl->def == XML_ATTRIBUTE_FIXED) {
	if (!xmlStrEqual(value, attrDecl->defaultValue)) {
	    xmlErrValidNode(ctxt, elem, XML_DTD_ATTRIBUTE_DEFAULT,
	   "Value for attribute %s of %s is different from default \"%s\"\n",
		   attr->name, elem->name, attrDecl->defaultValue);
	    ret = 0;
	}
    }

    /* Validity Constraint: ID uniqueness */
    if (attrDecl->atype == XML_ATTRIBUTE_ID) {
        if (xmlAddID(ctxt, doc, value, attr) == NULL)
	    ret = 0;
    }

    if ((attrDecl->atype == XML_ATTRIBUTE_IDREF) ||
	(attrDecl->atype == XML_ATTRIBUTE_IDREFS)) {
        if (xmlAddRef(ctxt, doc, value, attr) == NULL)
	    ret = 0;
    }

    /* Validity Constraint: Notation Attributes */
    if (attrDecl->atype == XML_ATTRIBUTE_NOTATION) {
        xmlEnumerationPtr tree = attrDecl->tree;
        xmlNotationPtr nota;

        /* First check that the given NOTATION was declared */
	nota = xmlGetDtdNotationDesc(doc->intSubset, value);
	if (nota == NULL)
	    nota = xmlGetDtdNotationDesc(doc->extSubset, value);

	if (nota == NULL) {
	    xmlErrValidNode(ctxt, elem, XML_DTD_UNKNOWN_NOTATION,
       "Value \"%s\" for attribute %s of %s is not a declared Notation\n",
		   value, attr->name, elem->name);
	    ret = 0;
        }

	/* Second, verify that it's among the list */
	while (tree != NULL) {
	    if (xmlStrEqual(tree->name, value)) break;
	    tree = tree->next;
	}
	if (tree == NULL) {
	    xmlErrValidNode(ctxt, elem, XML_DTD_NOTATION_VALUE,
"Value \"%s\" for attribute %s of %s is not among the enumerated notations\n",
		   value, attr->name, elem->name);
	    ret = 0;
	}
    }

    /* Validity Constraint: Enumeration */
    if (attrDecl->atype == XML_ATTRIBUTE_ENUMERATION) {
        xmlEnumerationPtr tree = attrDecl->tree;
	while (tree != NULL) {
	    if (xmlStrEqual(tree->name, value)) break;
	    tree = tree->next;
	}
	if (tree == NULL) {
	    xmlErrValidNode(ctxt, elem, XML_DTD_ATTRIBUTE_VALUE,
       "Value \"%s\" for attribute %s of %s is not among the enumerated set\n",
		   value, attr->name, elem->name);
	    ret = 0;
	}
    }

    /* Fixed Attribute Default */
    if ((attrDecl->def == XML_ATTRIBUTE_FIXED) &&
        (!xmlStrEqual(attrDecl->defaultValue, value))) {
	xmlErrValidNode(ctxt, elem, XML_DTD_ATTRIBUTE_VALUE,
	   "Value for attribute %s of %s must be \"%s\"\n",
	       attr->name, elem->name, attrDecl->defaultValue);
        ret = 0;
    }

    /* Extra check for the attribute value */
    ret &= xmlValidateAttributeValue2(ctxt, doc, attr->name,
				      attrDecl->atype, value);

    return(ret);
}