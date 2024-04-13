xsltNumberFormatGetMultipleLevel(xsltTransformContextPtr context,
				 xmlNodePtr node,
				 xsltCompMatchPtr countPat,
				 xsltCompMatchPtr fromPat,
				 double *array,
				 int max)
{
    int amount = 0;
    int cnt;
    xmlNodePtr oldCtxtNode;
    xmlNodePtr ancestor;
    xmlNodePtr preceding;
    xmlXPathParserContextPtr parser;

    oldCtxtNode = context->xpathCtxt->node;
    parser = xmlXPathNewParserContext(NULL, context->xpathCtxt);
    if (parser) {
	/* ancestor-or-self::*[count] */
	ancestor = node;
	while ((ancestor != NULL) && (ancestor->type != XML_DOCUMENT_NODE)) {
	    if ((fromPat != NULL) &&
		xsltTestCompMatchList(context, ancestor, fromPat))
		break; /* for */

            /*
             * The xmlXPathNext* iterators require that the context node is
             * set to the start node. Calls to xsltTestCompMatch* may also
             * leave the context node in an undefined state, so make sure
             * that the context node is reset before each iterator invocation.
             */

	    if (xsltTestCompMatchCount(context, ancestor, countPat, node)) {
		/* count(preceding-sibling::*) */
		cnt = 1;
                context->xpathCtxt->node = ancestor;
                preceding = xmlXPathNextPrecedingSibling(parser, ancestor);
                while (preceding != NULL) {
	            if (xsltTestCompMatchCount(context, preceding, countPat,
                                               node))
			cnt++;
                    context->xpathCtxt->node = ancestor;
                    preceding =
                        xmlXPathNextPrecedingSibling(parser, preceding);
		}
		array[amount++] = (double)cnt;
		if (amount >= max)
		    break; /* for */
	    }
            context->xpathCtxt->node = node;
            ancestor = xmlXPathNextAncestor(parser, ancestor);
	}
	xmlXPathFreeParserContext(parser);
    }
    context->xpathCtxt->node = oldCtxtNode;
    return amount;
}