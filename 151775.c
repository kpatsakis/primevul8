void Type_vcgt_Free(struct _cms_typehandler_struct* self, void* Ptr)
{
    cmsFreeToneCurveTriple((cmsToneCurve**) Ptr);
    _cmsFree(self ->ContextID, Ptr);
}