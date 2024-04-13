void FreeArray( _cmsDICarray* a)
{
    if (a ->Name.Offsets != NULL) FreeElem(&a->Name);
    if (a ->Value.Offsets != NULL) FreeElem(&a ->Value);
    if (a ->DisplayName.Offsets != NULL) FreeElem(&a->DisplayName);
    if (a ->DisplayValue.Offsets != NULL) FreeElem(&a ->DisplayValue);
}