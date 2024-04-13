cmsHPROFILE CreatePCS2ITU_ICC(void)
{
    cmsHPROFILE hProfile;
    cmsPipeline* BToA0;
    cmsStage* ColorMap;

    BToA0 = cmsPipelineAlloc(0, 3, 3);
    if (BToA0 == NULL) return NULL;

    ColorMap = cmsStageAllocCLut16bit(0, GRID_POINTS, 3, 3, NULL);
    if (ColorMap == NULL) return NULL;

    cmsPipelineInsertStage(BToA0, cmsAT_BEGIN, ColorMap);
    cmsStageSampleCLut16bit(ColorMap, PCS2ITU, NULL, 0);

    hProfile = cmsCreateProfilePlaceholder(0);
    if (hProfile == NULL) {
        cmsPipelineFree(BToA0);
        return NULL;
    }

    cmsWriteTag(hProfile, cmsSigBToA0Tag, BToA0);
    cmsSetColorSpace(hProfile, cmsSigLabData);
    cmsSetPCS(hProfile, cmsSigLabData);
    cmsSetDeviceClass(hProfile, cmsSigColorSpaceClass);

    cmsPipelineFree(BToA0);

    return hProfile;
}