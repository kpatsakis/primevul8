smtp_user_msg(uschar *code, uschar *user_msg)
{
int len = 3;
smtp_message_code(&code, &len, &user_msg, NULL, TRUE);
smtp_respond(code, len, TRUE, user_msg);
}