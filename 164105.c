htmlElementAllowedHere(const htmlElemDesc* parent, const xmlChar* elt) {
  const char** p ;

  if ( ! elt || ! parent || ! parent->subelts )
	return 0 ;

  for ( p = parent->subelts; *p; ++p )
    if ( !xmlStrcmp((const xmlChar *)*p, elt) )
      return 1 ;

  return 0 ;
}