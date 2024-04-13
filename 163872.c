int GetProfileColorSpace(cmsHPROFILE hProfile)
{
    cmsColorSpaceSignature ProfileSpace = cmsGetColorSpace(hProfile);

	return _cmsLCMScolorSpace(ProfileSpace);
}