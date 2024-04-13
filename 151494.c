print_unres_schema_item_fail(void *item, enum UNRES_ITEM type, void *str_node)
{
    struct lyxml_elem *xml;
    struct lyxml_attr *attr;
    struct unres_iffeat_data *iff_data;
    const char *name = NULL;
    struct unres_ext *extinfo;

    switch (type) {
    case UNRES_IDENT:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "identity", (char *)str_node);
        break;
    case UNRES_TYPE_IDENTREF:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "identityref", (char *)str_node);
        break;
    case UNRES_TYPE_LEAFREF:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "leafref",
               ((struct lys_type *)item)->info.lref.path);
        break;
    case UNRES_TYPE_DER_EXT:
    case UNRES_TYPE_DER_TPDF:
    case UNRES_TYPE_DER:
        xml = (struct lyxml_elem *)((struct lys_type *)item)->der;
        if (xml->flags & LY_YANG_STRUCTURE_FLAG) {
            name = ((struct yang_type *)xml)->name;
        } else {
            LY_TREE_FOR(xml->attr, attr) {
                if ((attr->type == LYXML_ATTR_STD) && !strcmp(attr->name, "name")) {
                    name = attr->value;
                    break;
                }
            }
            assert(attr);
        }
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "derived type", name);
        break;
    case UNRES_IFFEAT:
        iff_data = str_node;
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "if-feature", iff_data->fname);
        break;
    case UNRES_FEATURE:
        LOGVRB("There are unresolved if-features for \"%s\" feature circular dependency check, it will be attempted later",
               ((struct lys_feature *)item)->name);
        break;
    case UNRES_USES:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "uses", ((struct lys_node_uses *)item)->name);
        break;
    case UNRES_TYPEDEF_DFLT:
    case UNRES_TYPE_DFLT:
        if (*(char **)str_node) {
            LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "type default", *(char **)str_node);
        }   /* else no default value in the type itself, but we are checking some restrictions against
             *  possible default value of some base type. The failure is caused by not resolved base type,
             *  so it was already reported */
        break;
    case UNRES_CHOICE_DFLT:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "choice default", (char *)str_node);
        break;
    case UNRES_LIST_KEYS:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "list keys", (char *)str_node);
        break;
    case UNRES_LIST_UNIQ:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "list unique", (char *)str_node);
        break;
    case UNRES_AUGMENT:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "augment target",
               ((struct lys_node_augment *)item)->target_name);
        break;
    case UNRES_XPATH:
        LOGVRB("Resolving %s \"%s\" failed, it will be attempted later.", "XPath expressions of",
               ((struct lys_node *)item)->name);
        break;
    case UNRES_EXT:
        extinfo = (struct unres_ext *)str_node;
        name = extinfo->datatype == LYS_IN_YIN ? extinfo->data.yin->name : NULL; /* TODO YANG extension */
        LOGVRB("Resolving extension \"%s\" failed, it will be attempted later.", name);
        break;
    default:
        LOGINT(NULL);
        break;
    }
}