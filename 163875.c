cmsUInt32Number ComputeOutputFormatDescriptor(cmsUInt32Number dwInput, int OutColorSpace)
{
	int IsPlanar  = T_PLANAR(dwInput);
	int Channels  = 0;
	int Flavor    = 0;

	switch (OutColorSpace) {

   case PT_GRAY:
	   Channels = 1;
	   break;
   case PT_RGB:
   case PT_CMY:
   case PT_Lab:
   case PT_YUV:
   case PT_YCbCr:
	   Channels = 3;
	   break;

   case PT_CMYK:
	   if (Compressor.write_Adobe_marker)   // Adobe keeps CMYK inverted, so change flavor to chocolate
		   Flavor = 1;
	   Channels = 4;
	   break;
   default:
	   FatalError("Unsupported output color space");
	}

	return (COLORSPACE_SH(OutColorSpace)|PLANAR_SH(IsPlanar)|CHANNELS_SH(Channels)|BYTES_SH(1)|FLAVOR_SH(Flavor));
}