htmlAttrAllowed(const htmlElemDesc* elt, const xmlChar* attr, int legacy) {
  const char** p ;

  if ( !elt || ! attr )
	return HTML_INVALID ;

  if ( elt->attrs_req )
    for ( p = elt->attrs_req; *p; ++p)
      if ( !xmlStrcmp((const xmlChar*)*p, attr) )
        return HTML_REQUIRED ;

  if ( elt->attrs_opt )
    for ( p = elt->attrs_opt; *p; ++p)
      if ( !xmlStrcmp((const xmlChar*)*p, attr) )
        return HTML_VALID ;

  if ( legacy && elt->attrs_depr )
    for ( p = elt->attrs_depr; *p; ++p)
      if ( !xmlStrcmp((const xmlChar*)*p, attr) )
        return HTML_DEPRECATED ;

  return HTML_INVALID ;
}