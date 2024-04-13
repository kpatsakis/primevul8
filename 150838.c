xmlValidDebug(xmlNodePtr cur, xmlElementContentPtr cont) {
    char expr[5000];

    expr[0] = 0;
    xmlGenericError(xmlGenericErrorContext, "valid: ");
    xmlValidPrintNodeList(cur);
    xmlGenericError(xmlGenericErrorContext, "against ");
    xmlSnprintfElementContent(expr, 5000, cont, 1);
    xmlGenericError(xmlGenericErrorContext, "%s\n", expr);
}