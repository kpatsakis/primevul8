cmsBool ReadEmbeddedText(struct _cms_typehandler_struct* self, cmsIOHANDLER* io, cmsMLU** mlu, cmsUInt32Number SizeOfTag)
{
    cmsTagTypeSignature  BaseType;
    cmsUInt32Number nItems;

    BaseType = _cmsReadTypeBase(io);

    switch (BaseType) {

       case cmsSigTextType:
           if (*mlu) cmsMLUfree(*mlu);
           *mlu = (cmsMLU*)Type_Text_Read(self, io, &nItems, SizeOfTag);
           return (*mlu != NULL);

       case cmsSigTextDescriptionType:
           if (*mlu) cmsMLUfree(*mlu);
           *mlu =  (cmsMLU*) Type_Text_Description_Read(self, io, &nItems, SizeOfTag);
           return (*mlu != NULL);

           /*
           TBD: Size is needed for MLU, and we have no idea on which is the available size
           */

       case cmsSigMultiLocalizedUnicodeType:
           if (*mlu) cmsMLUfree(*mlu);
           *mlu =  (cmsMLU*) Type_MLU_Read(self, io, &nItems, SizeOfTag);
           return (*mlu != NULL);

       default: return FALSE;
    }
}