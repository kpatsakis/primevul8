snmpHandleUdp(int sock, void *)
{
    static char buf[SNMP_REQUEST_SIZE];
    Ip::Address from;
    SnmpRequest *snmp_rq;
    int len;

    debugs(49, 5, "snmpHandleUdp: Called.");

    Comm::SetSelect(sock, COMM_SELECT_READ, snmpHandleUdp, NULL, 0);

    memset(buf, '\0', sizeof(buf));

    len = comm_udp_recvfrom(sock, buf, sizeof(buf)-1, 0, from);

    if (len > 0) {
        debugs(49, 3, "snmpHandleUdp: FD " << sock << ": received " << len << " bytes from " << from << ".");

        snmp_rq = (SnmpRequest *)xcalloc(1, sizeof(SnmpRequest));
        snmp_rq->buf = (u_char *) buf;
        snmp_rq->len = len;
        snmp_rq->sock = sock;
        snmp_rq->outbuf = (unsigned char *)xmalloc(snmp_rq->outlen = SNMP_REQUEST_SIZE);
        snmp_rq->from = from;
        snmpDecodePacket(snmp_rq);
        xfree(snmp_rq->outbuf);
        xfree(snmp_rq);
    } else {
        int xerrno = errno;
        debugs(49, DBG_IMPORTANT, "snmpHandleUdp: FD " << sock << " recvfrom: " << xstrerr(xerrno));
    }
}