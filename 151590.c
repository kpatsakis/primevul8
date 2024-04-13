cmsBool  SaveDescription(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsMLU* Text)
{
    if (self ->ICCVersion < 0x4000000) {

        if (!_cmsWriteTypeBase(io, cmsSigTextDescriptionType)) return FALSE;
        return Type_Text_Description_Write(self, io, Text, 1);
    }
    else {
        if (!_cmsWriteTypeBase(io, cmsSigMultiLocalizedUnicodeType)) return FALSE;
        return Type_MLU_Write(self, io, Text, 1);
    }
}