smtp_rset_handler(void)
{
HAD(SCH_RSET);
incomplete_transaction_log(US"RSET");
smtp_printf("250 Reset OK\r\n", FALSE);
cmd_list[CMD_LIST_RSET].is_mail_cmd = FALSE;
}