gdAlphaBlendColor( int b1, int b2, int a1, int a2 )
{
	int c;
	int w;

	/* deal with special cases */

	if( (gdAlphaMax == a1) || (0 == a2) ) {
		/* the back pixel can't be seen */
		return b1;
	} else if(0 == a1) {
		/* the front pixel can't be seen */
		return b2;
	} else if(gdAlphaMax == a2) {
		/* the back pixel is opaque */
		return ( a1 * b1 + ( gdAlphaMax - a1 ) * b2 ) / gdAlphaMax;
	}

	/* the general case */
	w = ( a1 * ( gdAlphaMax - a2 ) / ( gdAlphaMax - a1 * a2 / gdAlphaMax ) * b1 + \
	 	  a2 * ( gdAlphaMax - a1 ) / ( gdAlphaMax - a1 * a2 / gdAlphaMax ) * b2 ) / gdAlphaMax;
	c = (a2 * b2  +  ( gdAlphaMax - a2 ) * w ) / gdAlphaMax;
	return ( a1 * b1 + ( gdAlphaMax - a1 ) * c ) / gdAlphaMax;
}