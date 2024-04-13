poppler_document_get_permissions (PopplerDocument *document)
{
  guint flag = 0;

  g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), POPPLER_PERMISSIONS_FULL);

  if (document->doc->okToPrint ())
    flag |= POPPLER_PERMISSIONS_OK_TO_PRINT;
  if (document->doc->okToChange ())
    flag |= POPPLER_PERMISSIONS_OK_TO_MODIFY;
  if (document->doc->okToCopy ())
    flag |= POPPLER_PERMISSIONS_OK_TO_COPY;
  if (document->doc->okToAddNotes ())
    flag |= POPPLER_PERMISSIONS_OK_TO_ADD_NOTES;
  if (document->doc->okToFillForm ())
    flag |= POPPLER_PERMISSIONS_OK_TO_FILL_FORM;
  if (document->doc->okToAccessibility())
    flag |= POPPLER_PERMISSIONS_OK_TO_EXTRACT_CONTENTS;
  if (document->doc->okToAssemble())
    flag |= POPPLER_PERMISSIONS_OK_TO_ASSEMBLE;
  if (document->doc->okToPrintHighRes())
    flag |= POPPLER_PERMISSIONS_OK_TO_PRINT_HIGH_RESOLUTION;

  return (PopplerPermissions)flag;
}