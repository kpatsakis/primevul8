static void red_sasl_auth_free(RedSASLAuth *auth)
{
    g_free(auth->data);
    g_free(auth->mechname);
    g_free(auth->mechlist);
    g_free(auth);
}