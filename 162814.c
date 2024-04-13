extract_tag_der_recursive(asn1_node node, const unsigned char *der, int der_len,
		       int *ret_len, int *inner_len, unsigned flags)
{
asn1_node p;
int ris = ASN1_DER_ERROR;

  if (type_field (node->type) == ASN1_ETYPE_CHOICE)
    {
      p = node->down;
      while (p)
        {
          ris = _asn1_extract_tag_der (p, der, der_len, ret_len, inner_len, flags);
          if (ris == ASN1_SUCCESS)
            break;
          p = p->right;
	}

      *ret_len = 0;
      return ris;
    }
  else
    return _asn1_extract_tag_der (node, der, der_len, ret_len, inner_len, flags);
}