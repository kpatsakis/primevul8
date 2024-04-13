htmlNodeStatus(const htmlNodePtr node, int legacy) {
  if ( ! node )
    return HTML_INVALID ;

  switch ( node->type ) {
    case XML_ELEMENT_NODE:
      return legacy
	? ( htmlElementAllowedHere (
		htmlTagLookup(node->parent->name) , node->name
		) ? HTML_VALID : HTML_INVALID )
	: htmlElementStatusHere(
		htmlTagLookup(node->parent->name) ,
		htmlTagLookup(node->name) )
	;
    case XML_ATTRIBUTE_NODE:
      return htmlAttrAllowed(
	htmlTagLookup(node->parent->name) , node->name, legacy) ;
    default: return HTML_NA ;
  }
}