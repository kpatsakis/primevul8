xsltFormatNumberFunction(xmlXPathParserContextPtr ctxt, int nargs)
{
    xmlXPathObjectPtr numberObj = NULL;
    xmlXPathObjectPtr formatObj = NULL;
    xmlXPathObjectPtr decimalObj = NULL;
    xsltStylesheetPtr sheet;
    xsltDecimalFormatPtr formatValues;
    xmlChar *result;
    xsltTransformContextPtr tctxt;

    tctxt = xsltXPathGetTransformContext(ctxt);
    if (tctxt == NULL)
	return;
    sheet = tctxt->style;
    if (sheet == NULL)
	return;
    formatValues = sheet->decimalFormat;

    switch (nargs) {
    case 3:
	CAST_TO_STRING;
	decimalObj = valuePop(ctxt);
	formatValues = xsltDecimalFormatGetByName(sheet, decimalObj->stringval);
	if (formatValues == NULL) {
	    xsltTransformError(tctxt, NULL, NULL,
		    "format-number() : undeclared decimal format '%s'\n",
		    decimalObj->stringval);
	}
	/* Intentional fall-through */
    case 2:
	CAST_TO_STRING;
	formatObj = valuePop(ctxt);
	CAST_TO_NUMBER;
	numberObj = valuePop(ctxt);
	break;
    default:
	XP_ERROR(XPATH_INVALID_ARITY);
    }

    if (formatValues != NULL) {
	if (xsltFormatNumberConversion(formatValues,
				       formatObj->stringval,
				       numberObj->floatval,
				       &result) == XPATH_EXPRESSION_OK) {
	    valuePush(ctxt, xmlXPathNewString(result));
	    xmlFree(result);
	}
    }

    xmlXPathFreeObject(numberObj);
    xmlXPathFreeObject(formatObj);
    xmlXPathFreeObject(decimalObj);
}