void color_cielab_to_rgb(opj_image_t *image)
{
	int *row;
	int enumcs, numcomps;
	OPJ_COLOR_SPACE new_space;

	numcomps = (int)image->numcomps;
	
	if(numcomps != 3)
	{
		fprintf(stderr,"%s:%d:\n\tnumcomps %d not handled. Quitting.\n",
						__FILE__,__LINE__,numcomps);
		return;
	}
	
	row = (int*)image->icc_profile_buf;
	enumcs = row[0];
	
	if(enumcs == 14) /* CIELab */
	{
		int *L, *a, *b, *red, *green, *blue;
		int *src0, *src1, *src2, *dst0, *dst1, *dst2;
		double rl, ol, ra, oa, rb, ob, prec0, prec1, prec2;
		double minL, maxL, mina, maxa, minb, maxb;
		unsigned int default_type;
		unsigned int i, max;
		cmsHPROFILE in, out;
		cmsHTRANSFORM transform;
		cmsUInt16Number RGB[3];
		cmsCIELab Lab;
		
		in = cmsCreateLab4Profile(NULL);
		if(in == NULL){
			return;
		}
		out = cmsCreate_sRGBProfile();
		if(out == NULL){
			cmsCloseProfile(in);
			return;
		}
		transform = cmsCreateTransform(in, TYPE_Lab_DBL, out, TYPE_RGB_16, INTENT_PERCEPTUAL, 0);
		
#ifdef OPJ_HAVE_LIBLCMS2
		cmsCloseProfile(in);
		cmsCloseProfile(out);
#endif
		if(transform == NULL)
		{
#ifdef OPJ_HAVE_LIBLCMS1
			cmsCloseProfile(in);
			cmsCloseProfile(out);
#endif
			return;
		}
		new_space = OPJ_CLRSPC_SRGB;

		prec0 = (double)image->comps[0].prec;
		prec1 = (double)image->comps[1].prec;
		prec2 = (double)image->comps[2].prec;
		
		default_type = (unsigned int)row[1];
		
		if(default_type == 0x44454600)/* DEF : default */
		{
			rl = 100; ra = 170; rb = 200;
			ol = 0;
			oa = pow(2, prec1 - 1);
			ob = pow(2, prec2 - 2) +  pow(2, prec2 - 3);
		}
		else
		{
			rl = row[2]; ra = row[4]; rb = row[6];
			ol = row[3]; oa = row[5]; ob = row[7];
		}
		
		L = src0 = image->comps[0].data;
		a = src1 = image->comps[1].data;
		b = src2 = image->comps[2].data;
		
		max = image->comps[0].w * image->comps[0].h;
		
		red = dst0 = (int*)malloc(max * sizeof(int));
		green = dst1 = (int*)malloc(max * sizeof(int));
		blue = dst2 = (int*)malloc(max * sizeof(int));

		if(red == NULL || green == NULL || blue == NULL) goto fails;

		minL = -(rl * ol)/(pow(2, prec0)-1);
		maxL = minL + rl;
		
		mina = -(ra * oa)/(pow(2, prec1)-1);
		maxa = mina + ra;
		
		minb = -(rb * ob)/(pow(2, prec2)-1);
		maxb = minb + rb;
		
		for(i = 0; i < max; ++i)
		{
			Lab.L = minL + (double)(*L) * (maxL - minL)/(pow(2, prec0)-1); ++L;
			Lab.a = mina + (double)(*a) * (maxa - mina)/(pow(2, prec1)-1); ++a;
			Lab.b = minb + (double)(*b) * (maxb - minb)/(pow(2, prec2)-1); ++b;
		
			cmsDoTransform(transform, &Lab, RGB, 1);
		
			*red++ = RGB[0];
			*green++ = RGB[1];
			*blue++ = RGB[2];
		}
		cmsDeleteTransform(transform);
#ifdef OPJ_HAVE_LIBLCMS1
		cmsCloseProfile(in);
		cmsCloseProfile(out);
#endif
		free(src0); image->comps[0].data = dst0;
		free(src1); image->comps[1].data = dst1;
		free(src2); image->comps[2].data = dst2;
		
		image->color_space = new_space;
		image->comps[0].prec = 16;
		image->comps[1].prec = 16;
		image->comps[2].prec = 16;
		
		return;

fails:
		cmsDeleteTransform(transform);
#ifdef OPJ_HAVE_LIBLCMS1
		cmsCloseProfile(in);
		cmsCloseProfile(out);
#endif
		if(red) free(red);
		if(green) free(green);
		if(blue) free(blue);
		return;
	}
	
	fprintf(stderr,"%s:%d:\n\tenumCS %d not handled. Ignoring.\n", __FILE__,__LINE__, enumcs);
}/* color_cielab_to_rgb() */