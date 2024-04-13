smtp_reset(void *reset_point)
{
recipients_list = NULL;
rcpt_count = rcpt_defer_count = rcpt_fail_count =
  raw_recipients_count = recipients_count = recipients_list_max = 0;
message_linecount = 0;
message_size = -1;
acl_added_headers = NULL;
acl_removed_headers = NULL;
f.queue_only_policy = FALSE;
rcpt_smtp_response = NULL;
fl.rcpt_smtp_response_same = TRUE;
fl.rcpt_in_progress = FALSE;
f.deliver_freeze = FALSE;                              /* Can be set by ACL */
freeze_tell = freeze_tell_config;                    /* Can be set by ACL */
fake_response = OK;                                  /* Can be set by ACL */
#ifdef WITH_CONTENT_SCAN
f.no_mbox_unspool = FALSE;                             /* Can be set by ACL */
#endif
f.submission_mode = FALSE;                             /* Can be set by ACL */
f.suppress_local_fixups = f.suppress_local_fixups_default; /* Can be set by ACL */
f.active_local_from_check = local_from_check;          /* Can be set by ACL */
f.active_local_sender_retain = local_sender_retain;    /* Can be set by ACL */
sending_ip_address = NULL;
return_path = sender_address = NULL;
sender_data = NULL;				     /* Can be set by ACL */
deliver_localpart_parent = deliver_localpart_orig = NULL;
deliver_domain_parent = deliver_domain_orig = NULL;
callout_address = NULL;
submission_name = NULL;                              /* Can be set by ACL */
raw_sender = NULL;                  /* After SMTP rewrite, before qualifying */
sender_address_unrewritten = NULL;  /* Set only after verify rewrite */
sender_verified_list = NULL;        /* No senders verified */
memset(sender_address_cache, 0, sizeof(sender_address_cache));
memset(sender_domain_cache, 0, sizeof(sender_domain_cache));

authenticated_sender = NULL;
#ifdef EXPERIMENTAL_BRIGHTMAIL
bmi_run = 0;
bmi_verdicts = NULL;
#endif
dnslist_domain = dnslist_matched = NULL;
#ifdef SUPPORT_SPF
spf_header_comment = spf_received = spf_result = spf_smtp_comment = NULL;
spf_result_guessed = FALSE;
#endif
#ifndef DISABLE_DKIM
dkim_cur_signer = dkim_signers =
dkim_signing_domain = dkim_signing_selector = dkim_signatures = NULL;
dkim_cur_signer = dkim_signers = dkim_signing_domain = dkim_signing_selector = NULL;
f.dkim_disable_verify = FALSE;
dkim_collect_input = 0;
dkim_verify_overall = dkim_verify_status = dkim_verify_reason = NULL;
dkim_key_length = 0;
dkim_verify_signers = US"$dkim_signers";
#endif
#ifdef EXPERIMENTAL_DMARC
f.dmarc_has_been_checked = f.dmarc_disable_verify = f.dmarc_enable_forensic = FALSE;
dmarc_domain_policy = dmarc_status = dmarc_status_text =
dmarc_used_domain = NULL;
#endif
#ifdef EXPERIMENTAL_ARC
arc_state = arc_state_reason = NULL;
#endif
dsn_ret = 0;
dsn_envid = NULL;
deliver_host = deliver_host_address = NULL;	/* Can be set by ACL */
#ifndef DISABLE_PRDR
prdr_requested = FALSE;
#endif
#ifdef SUPPORT_I18N
message_smtputf8 = FALSE;
#endif
body_linecount = body_zerocount = 0;

sender_rate = sender_rate_limit = sender_rate_period = NULL;
ratelimiters_mail = NULL;           /* Updated by ratelimit ACL condition */
                   /* Note that ratelimiters_conn persists across resets. */

/* Reset message ACL variables */

acl_var_m = NULL;

/* The message body variables use malloc store. They may be set if this is
not the first message in an SMTP session and the previous message caused them
to be referenced in an ACL. */

if (message_body)
  {
  store_free(message_body);
  message_body = NULL;
  }

if (message_body_end)
  {
  store_free(message_body_end);
  message_body_end = NULL;
  }

/* Warning log messages are also saved in malloc store. They are saved to avoid
repetition in the same message, but it seems right to repeat them for different
messages. */

while (acl_warn_logged)
  {
  string_item *this = acl_warn_logged;
  acl_warn_logged = acl_warn_logged->next;
  store_free(this);
  }
store_reset(reset_point);
}