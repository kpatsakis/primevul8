asn1_read_tag (asn1_node root, const char *name, int *tagValue,
	       int *classValue)
{
  asn1_node node, p, pTag;

  node = asn1_find_node (root, name);
  if (node == NULL)
    return ASN1_ELEMENT_NOT_FOUND;

  p = node->down;

  /* pTag will points to the IMPLICIT TAG */
  pTag = NULL;
  if (node->type & CONST_TAG)
    {
      while (p)
	{
	  if (type_field (p->type) == ASN1_ETYPE_TAG)
	    {
	      if ((p->type & CONST_IMPLICIT) && (pTag == NULL))
		pTag = p;
	      else if (p->type & CONST_EXPLICIT)
		pTag = NULL;
	    }
	  p = p->right;
	}
    }

  if (pTag)
    {
      *tagValue = _asn1_strtoul (pTag->value, NULL, 10);

      if (pTag->type & CONST_APPLICATION)
	*classValue = ASN1_CLASS_APPLICATION;
      else if (pTag->type & CONST_UNIVERSAL)
	*classValue = ASN1_CLASS_UNIVERSAL;
      else if (pTag->type & CONST_PRIVATE)
	*classValue = ASN1_CLASS_PRIVATE;
      else
	*classValue = ASN1_CLASS_CONTEXT_SPECIFIC;
    }
  else
    {
      unsigned type = type_field (node->type);
      *classValue = ASN1_CLASS_UNIVERSAL;

      switch (type)
	{
	CASE_HANDLED_ETYPES:
	  *tagValue = _asn1_tags[type].tag;
	  break;
	case ASN1_ETYPE_TAG:
	case ASN1_ETYPE_CHOICE:
	case ASN1_ETYPE_ANY:
	  *tagValue = -1;
	  break;
	default:
	  break;
	}
    }

  return ASN1_SUCCESS;
}