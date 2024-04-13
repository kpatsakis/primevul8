ldns_str2rdf_hip(ldns_rdf **rd, const char *str)
{
	const char *hit = strchr(str, ' ') + 1;
	const char *pk  = hit == NULL ? NULL : strchr(hit, ' ') + 1;
	size_t hit_size = hit == NULL ? 0
	                : pk  == NULL ? strlen(hit) : (size_t) (pk - hit) - 1;
	size_t  pk_size = pk  == NULL ? 0 : strlen(pk);
	size_t hit_wire_size = (hit_size + 1) / 2;
	size_t  pk_wire_size = ldns_b64_pton_calculate_size(pk_size);
	size_t rdf_size = 4 + hit_wire_size + pk_wire_size;

	char *endptr; /* utility var for strtol usage */
	int algorithm = strtol(str, &endptr, 10);

	uint8_t *data, *dp;
	int hi, lo, written;

	if (hit_size == 0 || pk_size == 0 || (hit_size + 1) / 2 > 255
			|| rdf_size > LDNS_MAX_RDFLEN
			|| algorithm < 0 || algorithm > 255
			|| (errno != 0 && algorithm == 0) /* out of range */
			|| endptr == str                  /* no digits    */) {

		return LDNS_STATUS_SYNTAX_ERR;
	}
	if ((data = LDNS_XMALLOC(uint8_t, rdf_size)) == NULL) {

		return LDNS_STATUS_MEM_ERR;
	}
	/* From RFC 5205 section 5. HIP RR Storage Format:
	 *************************************************

	0                   1                   2                   3
	0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|  HIT length   | PK algorithm  |          PK length            |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                                                               |
	~                           HIT                                 ~
	|                                                               |
	+                     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                     |                                         |
	+-+-+-+-+-+-+-+-+-+-+-+                                         +
	|                           Public Key                          |
	~                                                               ~
	|                                                               |
	+                               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|                               |                               |
	+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+                               +
	|                                                               |
	~                       Rendezvous Servers                      ~
	|                                                               |
	+             +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	|             |
	+-+-+-+-+-+-+-+                                                    */

	data[0] = (uint8_t) hit_wire_size;
	data[1] = (uint8_t) algorithm;

	for (dp = data + 4; *hit && *hit != ' '; dp++) {

		if ((hi = ldns_hexdigit_to_int(*hit++)) == -1 ||
		    (lo = ldns_hexdigit_to_int(*hit++)) == -1) {

			LDNS_FREE(data);
			return LDNS_STATUS_INVALID_HEX;
		}
		*dp = (uint8_t) hi << 4 | lo;
	}
	if ((written = ldns_b64_pton(pk, dp, pk_wire_size)) <= 0) {

		LDNS_FREE(data);
		return LDNS_STATUS_INVALID_B64;
	}

	/* Because ldns_b64_pton_calculate_size isn't always correct:
	 * (we have to fix it at some point)
	 */
	pk_wire_size = (uint16_t) written;
	ldns_write_uint16(data + 2, pk_wire_size);
	rdf_size = 4 + hit_wire_size + pk_wire_size;

	/* Create rdf */
	if (! (*rd = ldns_rdf_new(LDNS_RDF_TYPE_HIP, rdf_size, data))) {

		LDNS_FREE(data);
		return LDNS_STATUS_MEM_ERR;
	}
	return LDNS_STATUS_OK;
}