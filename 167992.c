_log_connection_sort_names_fcn(gconstpointer a, gconstpointer b)
{
    const LogConnectionSettingItem *v1 = a;
    const LogConnectionSettingItem *v2 = b;

    /* we want to first show the items, that disappeared, then the one that changed and
     * then the ones that were added. */

    if ((v1->diff_result & NM_SETTING_DIFF_RESULT_IN_A)
        != (v2->diff_result & NM_SETTING_DIFF_RESULT_IN_A))
        return (v1->diff_result & NM_SETTING_DIFF_RESULT_IN_A) ? -1 : 1;
    if ((v1->diff_result & NM_SETTING_DIFF_RESULT_IN_B)
        != (v2->diff_result & NM_SETTING_DIFF_RESULT_IN_B))
        return (v1->diff_result & NM_SETTING_DIFF_RESULT_IN_B) ? 1 : -1;
    return strcmp(v1->item_name, v2->item_name);
}