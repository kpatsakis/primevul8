int ITU2PCS( register const cmsUInt16Number In[], register cmsUInt16Number Out[], register void*  Cargo)
{
	cmsCIELab Lab;

	ITU2Lab(In, &Lab);
	cmsFloat2LabEncoded(Out, &Lab);
	return TRUE;

    UTILS_UNUSED_PARAMETER(Cargo);
}