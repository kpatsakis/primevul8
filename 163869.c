cmsHPROFILE CreateITU2PCS_ICC(void)
{
	cmsHPROFILE hProfile;
	cmsPipeline* AToB0;
	cmsStage* ColorMap;

	AToB0 = cmsPipelineAlloc(0, 3, 3);
	if (AToB0 == NULL) return NULL;

	ColorMap = cmsStageAllocCLut16bit(0, GRID_POINTS, 3, 3, NULL);
	if (ColorMap == NULL) return NULL;

    cmsPipelineInsertStage(AToB0, cmsAT_BEGIN, ColorMap);
	cmsStageSampleCLut16bit(ColorMap, ITU2PCS, NULL, 0);

	hProfile = cmsCreateProfilePlaceholder(0);
	if (hProfile == NULL) {
		cmsPipelineFree(AToB0);
		return NULL;
	}

	cmsWriteTag(hProfile, cmsSigAToB0Tag, AToB0);
	cmsSetColorSpace(hProfile, cmsSigLabData);
	cmsSetPCS(hProfile, cmsSigLabData);
	cmsSetDeviceClass(hProfile, cmsSigColorSpaceClass);
	cmsPipelineFree(AToB0);

	return hProfile;
}