void Type_ParametricCurve_Free(struct _cms_typehandler_struct* self, void* Ptr)
{
    cmsToneCurve* gamma = (cmsToneCurve*) Ptr;

    cmsFreeToneCurve(gamma);
    return;

    cmsUNUSED_PARAMETER(self);
}