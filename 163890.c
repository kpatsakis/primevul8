int PCS2ITU(register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void*  Cargo)
{
	cmsCIELab Lab;

	cmsLabEncoded2Float(&Lab, In);
	cmsDesaturateLab(&Lab, 85, -85, 125, -75);    // This function does the necessary gamut remapping
	Lab2ITU(&Lab, Out);
	return TRUE;

    UTILS_UNUSED_PARAMETER(Cargo);
}