gdFullAlphaBlend (int dst, int src)
{
	int a1, a2;
	a1 = gdAlphaTransparent - gdTrueColorGetAlpha(src);
	a2 = gdAlphaTransparent - gdTrueColorGetAlpha(dst);

	return ( ((gdAlphaTransparent - ((a1+a2)-(a1*a2/gdAlphaMax))) << 24) +
		(gdAlphaBlendColor( gdTrueColorGetRed(src), gdTrueColorGetRed(dst), a1, a2 ) << 16) +
		(gdAlphaBlendColor( gdTrueColorGetGreen(src), gdTrueColorGetGreen(dst), a1, a2 ) << 8) +
		(gdAlphaBlendColor( gdTrueColorGetBlue(src), gdTrueColorGetBlue(dst), a1, a2 ))
		);
}