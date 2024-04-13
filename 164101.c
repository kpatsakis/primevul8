htmlElementStatusHere(const htmlElemDesc* parent, const htmlElemDesc* elt) {
  if ( ! parent || ! elt )
    return HTML_INVALID ;
  if ( ! htmlElementAllowedHere(parent, (const xmlChar*) elt->name ) )
    return HTML_INVALID ;

  return ( elt->dtd == 0 ) ? HTML_VALID : HTML_DEPRECATED ;
}