authres_smtpauth(gstring * g)
{
if (!sender_host_authenticated)
  return g;

g = string_append(g, 2, US";\n\tauth=pass (", sender_host_auth_pubname);

if (Ustrcmp(sender_host_auth_pubname, "tls") != 0)
  g = string_append(g, 2, US") smtp.auth=", authenticated_id);
else if (authenticated_id)
  g = string_append(g, 2, US") x509.auth=", authenticated_id);
else
  g = string_catn(g, US") reason=x509.auth", 17);

if (authenticated_sender)
  g = string_append(g, 2, US" smtp.mailfrom=", authenticated_sender);
return g;
}