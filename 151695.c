void GenericMPEfree(struct _cms_typehandler_struct* self, void *Ptr)
{
    cmsStageFree((cmsStage*) Ptr);
    return;

    cmsUNUSED_PARAMETER(self);
}