int GetDevicelinkColorSpace(cmsHPROFILE hProfile)
{
    cmsColorSpaceSignature ProfileSpace = cmsGetPCS(hProfile);

	return _cmsLCMScolorSpace(ProfileSpace);
}