xsltSystemPropertyFunction(xmlXPathParserContextPtr ctxt, int nargs){
    xmlXPathObjectPtr obj;
    xmlChar *prefix, *name;
    const xmlChar *nsURI = NULL;

    if (nargs != 1) {
	xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
		"system-property() : expects one string arg\n");
	ctxt->error = XPATH_INVALID_ARITY;
	return;
    }
    if ((ctxt->value == NULL) || (ctxt->value->type != XPATH_STRING)) {
	xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
	    "system-property() : invalid arg expecting a string\n");
	ctxt->error = XPATH_INVALID_TYPE;
	return;
    }
    obj = valuePop(ctxt);
    if (obj->stringval == NULL) {
	valuePush(ctxt, xmlXPathNewString((const xmlChar *)""));
    } else {
	name = xmlSplitQName2(obj->stringval, &prefix);
	if (name == NULL) {
	    name = xmlStrdup(obj->stringval);
	} else {
	    nsURI = xmlXPathNsLookup(ctxt->context, prefix);
	    if (nsURI == NULL) {
		xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
		    "system-property() : prefix %s is not bound\n", prefix);
	    }
	}

	if (xmlStrEqual(nsURI, XSLT_NAMESPACE)) {
#ifdef DOCBOOK_XSL_HACK
	    if (xmlStrEqual(name, (const xmlChar *)"vendor")) {
		xsltStylesheetPtr sheet;
		xsltTransformContextPtr tctxt;

		tctxt = xsltXPathGetTransformContext(ctxt);
		if ((tctxt != NULL) && (tctxt->inst != NULL) &&
		    (xmlStrEqual(tctxt->inst->name, BAD_CAST "variable")) &&
		    (tctxt->inst->parent != NULL) &&
		    (xmlStrEqual(tctxt->inst->parent->name,
				 BAD_CAST "template")))
		    sheet = tctxt->style;
		else
		    sheet = NULL;
		if ((sheet != NULL) && (sheet->doc != NULL) &&
		    (sheet->doc->URL != NULL) &&
		    (xmlStrstr(sheet->doc->URL,
			       (const xmlChar *)"chunk") != NULL)) {
		    valuePush(ctxt, xmlXPathNewString(
			(const xmlChar *)"libxslt (SAXON 6.2 compatible)"));

		} else {
		    valuePush(ctxt, xmlXPathNewString(
			(const xmlChar *)XSLT_DEFAULT_VENDOR));
		}
	    } else
#else
	    if (xmlStrEqual(name, (const xmlChar *)"vendor")) {
		valuePush(ctxt, xmlXPathNewString(
			  (const xmlChar *)XSLT_DEFAULT_VENDOR));
	    } else
#endif
	    if (xmlStrEqual(name, (const xmlChar *)"version")) {
		valuePush(ctxt, xmlXPathNewString(
		    (const xmlChar *)XSLT_DEFAULT_VERSION));
	    } else if (xmlStrEqual(name, (const xmlChar *)"vendor-url")) {
		valuePush(ctxt, xmlXPathNewString(
		    (const xmlChar *)XSLT_DEFAULT_URL));
	    } else {
		valuePush(ctxt, xmlXPathNewString((const xmlChar *)""));
	    }
	} else {
	    valuePush(ctxt, xmlXPathNewString((const xmlChar *)""));
        }
	if (name != NULL)
	    xmlFree(name);
	if (prefix != NULL)
	    xmlFree(prefix);
    }
    xmlXPathFreeObject(obj);
}