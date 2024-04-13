get_time_value(proto_tree *tree, tvbuff_t *tvb, const gint start,
	       const gint length, const guint encoding, nstime_t *time_stamp,
	       const gboolean is_relative)
{
	guint32     tmpsecs;
	guint64     tmp64secs;
	guint64     todusecs;

	switch (encoding) {

		case ENC_TIME_SECS_NSECS|ENC_BIG_ENDIAN:
			/*
			 * If the length is 16, 8-byte seconds, followed
			 * by 8-byte fractional time in nanoseconds,
			 * both big-endian.
			 *
			 * If the length is 12, 8-byte seconds, followed
			 * by 4-byte fractional time in nanoseconds,
			 * both big-endian.
			 *
			 * If the length is 8, 4-byte seconds, followed
			 * by 4-byte fractional time in nanoseconds,
			 * both big-endian.
			 *
			 * For absolute times, the seconds are seconds
			 * since the UN*X epoch.
			 */
			if (length == 16) {
				time_stamp->secs  = (time_t)tvb_get_ntoh64(tvb, start);
				time_stamp->nsecs = (guint32)tvb_get_ntoh64(tvb, start+8);
			} else if (length == 12) {
				time_stamp->secs  = (time_t)tvb_get_ntoh64(tvb, start);
				time_stamp->nsecs = tvb_get_ntohl(tvb, start+8);
			} else if (length == 8) {
				time_stamp->secs  = (time_t)tvb_get_ntohl(tvb, start);
				time_stamp->nsecs = tvb_get_ntohl(tvb, start+4);
			} else if (length == 4) {
				/*
				 * Backwards compatibility.
				 * ENC_TIME_SECS_NSECS is 0; using
				 * ENC_BIG_ENDIAN by itself with a 4-byte
				 * time-in-seconds value was done in the
				 * past.
				 */
				time_stamp->secs  = (time_t)tvb_get_ntohl(tvb, start);
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a timespec", length, (length < 4));
			}
			break;

		case ENC_TIME_SECS_NSECS|ENC_LITTLE_ENDIAN:
			/*
			 * If the length is 16, 8-byte seconds, followed
			 * by 8-byte fractional time in nanoseconds,
			 * both little-endian.
			 *
			 * If the length is 12, 8-byte seconds, followed
			 * by 4-byte fractional time in nanoseconds,
			 * both little-endian.
			 *
			 * If the length is 8, 4-byte seconds, followed
			 * by 4-byte fractional time in nanoseconds,
			 * both little-endian.
			 *
			 * For absolute times, the seconds are seconds
			 * since the UN*X epoch.
			 */
			if (length == 16) {
				time_stamp->secs  = (time_t)tvb_get_letoh64(tvb, start);
				time_stamp->nsecs = (guint32)tvb_get_letoh64(tvb, start+8);
			} else if (length == 12) {
				time_stamp->secs  = (time_t)tvb_get_letoh64(tvb, start);
				time_stamp->nsecs = tvb_get_letohl(tvb, start+8);
			} else if (length == 8) {
				time_stamp->secs  = (time_t)tvb_get_letohl(tvb, start);
				time_stamp->nsecs = tvb_get_letohl(tvb, start+4);
			} else if (length == 4) {
				/*
				 * Backwards compatibility.
				 * ENC_TIME_SECS_NSECS is 0; using
				 * ENC_LITTLE_ENDIAN by itself with a 4-byte
				 * time-in-seconds value was done in the
				 * past.
				 */
				time_stamp->secs  = (time_t)tvb_get_letohl(tvb, start);
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a timespec", length, (length < 4));
			}
			break;

		case ENC_TIME_NTP|ENC_BIG_ENDIAN:
			/*
			 * NTP time stamp, big-endian.
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			/* We need a temporary variable here so the unsigned math
			 * works correctly (for years > 2036 according to RFC 2030
			 * chapter 3).
			 *
			 * If bit 0 is set, the UTC time is in the range 1968-2036 and
			 * UTC time is reckoned from 0h 0m 0s UTC on 1 January 1900.
			 * If bit 0 is not set, the time is in the range 2036-2104 and
			 * UTC time is reckoned from 6h 28m 16s UTC on 7 February 2036.
			 */
			tmpsecs  = tvb_get_ntohl(tvb, start);
			if ((tmpsecs & 0x80000000) != 0)
				time_stamp->secs = (time_t)((gint64)tmpsecs - NTP_TIMEDIFF1900TO1970SEC);
			else
				time_stamp->secs = (time_t)((gint64)tmpsecs + NTP_TIMEDIFF1970TO2036SEC);

			if (length == 8) {
				/*
				 * Convert 1/2^32s of a second to nanoseconds.
				 */
				time_stamp->nsecs = (int)(1000000000*(tvb_get_ntohl(tvb, start+4)/4294967296.0));
				if ((time_stamp->nsecs == 0) && (tmpsecs == 0)) {
					//This is "NULL" time
					time_stamp->secs = 0;
				}
			} else if (length == 4) {
				/*
				 * Backwards compatibility.
				 */
				if (tmpsecs == 0) {
					//This is "NULL" time
					time_stamp->secs = 0;
				}
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an NTP time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_NTP|ENC_LITTLE_ENDIAN:
			/*
			 * NTP time stamp, little-endian.
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			/* We need a temporary variable here so the unsigned math
			 * works correctly (for years > 2036 according to RFC 2030
			 * chapter 3).
			 *
			 * If bit 0 is set, the UTC time is in the range 1968-2036 and
			 * UTC time is reckoned from 0h 0m 0s UTC on 1 January 1900.
			 * If bit 0 is not set, the time is in the range 2036-2104 and
			 * UTC time is reckoned from 6h 28m 16s UTC on 7 February 2036.
			 */
			tmpsecs  = tvb_get_letohl(tvb, start);
			if ((tmpsecs & 0x80000000) != 0)
				time_stamp->secs = (time_t)((gint64)tmpsecs - NTP_TIMEDIFF1900TO1970SEC);
			else
				time_stamp->secs = (time_t)((gint64)tmpsecs + NTP_TIMEDIFF1970TO2036SEC);

			if (length == 8) {
				/*
				 * Convert 1/2^32s of a second to nanoseconds.
				 */
				time_stamp->nsecs = (int)(1000000000*(tvb_get_letohl(tvb, start+4)/4294967296.0));
				if ((time_stamp->nsecs == 0) && (tmpsecs == 0)) {
					//This is "NULL" time
					time_stamp->secs = 0;
				}
			} else if (length == 4) {
				/*
				 * Backwards compatibility.
				 */
				if (tmpsecs == 0) {
					//This is "NULL" time
					time_stamp->secs = 0;
				}
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an NTP time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_TOD|ENC_BIG_ENDIAN:
			/*
			 * S/3x0 and z/Architecture TOD clock time stamp,
			 * big-endian.  The epoch is January 1, 1900,
			 * 00:00:00 (proleptic?) UTC.
			 *
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);
			DISSECTOR_ASSERT(length == 8);

			if (length == 8) {
				todusecs  = tvb_get_ntoh64(tvb, start) >> 12;
				time_stamp->secs = (time_t)((todusecs  / 1000000) - EPOCH_DELTA_1900_01_01_00_00_00_UTC);
				time_stamp->nsecs = (int)((todusecs  % 1000000) * 1000);
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a TOD clock time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_TOD|ENC_LITTLE_ENDIAN:
			/*
			 * S/3x0 and z/Architecture TOD clock time stamp,
			 * little-endian.  The epoch is January 1, 1900,
			 * 00:00:00 (proleptic?) UTC.
			 *
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				todusecs  = tvb_get_letoh64(tvb, start) >> 12 ;
				time_stamp->secs = (time_t)((todusecs  / 1000000) - EPOCH_DELTA_1900_01_01_00_00_00_UTC);
				time_stamp->nsecs = (int)((todusecs  % 1000000) * 1000);
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a TOD clock time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_RTPS|ENC_BIG_ENDIAN:
			/*
			 * Time stamp using the same seconds/fraction format
			 * as NTP, but with the origin of the time stamp being
			 * the UNIX epoch rather than the NTP epoch; big-
			 * endian.
			 *
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				time_stamp->secs = (time_t)tvb_get_ntohl(tvb, start);
				/*
				 * Convert 1/2^32s of a second to nanoseconds.
				 */
				time_stamp->nsecs = (int)(1000000000*(tvb_get_ntohl(tvb, start+4)/4294967296.0));
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an RTPS time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_RTPS|ENC_LITTLE_ENDIAN:
			/*
			 * Time stamp using the same seconds/fraction format
			 * as NTP, but with the origin of the time stamp being
			 * the UNIX epoch rather than the NTP epoch; little-
			 * endian.
			 *
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				time_stamp->secs = (time_t)tvb_get_letohl(tvb, start);
				/*
				 * Convert 1/2^32s of a second to nanoseconds.
				 */
				time_stamp->nsecs = (int)(1000000000*(tvb_get_letohl(tvb, start+4)/4294967296.0));
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an RTPS time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_MIP6 | ENC_BIG_ENDIAN:
			/*
			* MIP6 time stamp, big-endian.
			* A 64-bit unsigned integer field containing a timestamp.  The
			* value indicates the number of seconds since January 1, 1970,
			* 00:00 UTC, by using a fixed point format.  In this format, the
			* integer number of seconds is contained in the first 48 bits of
			* the field, and the remaining 16 bits indicate the number of
			* 1/65536 fractions of a second.

			* Only supported for absolute times.
			*/
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				/* We need a temporary variable here so the casting and fractions
				* of a second work correctly.
				*/
				tmp64secs = tvb_get_ntoh48(tvb, start);
				tmpsecs = tvb_get_ntohs(tvb, start + 6);
				tmpsecs <<= 16;

				if ((tmp64secs == 0) && (tmpsecs == 0)) {
					//This is "NULL" time
					time_stamp->secs = 0;
					time_stamp->nsecs = 0;
				} else {
					time_stamp->secs = (time_t)tmp64secs;
					time_stamp->nsecs = (int)((tmpsecs / 4294967296.0) * 1000000000);
				}
			} else {
				time_stamp->secs = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an NTP time stamp", length, (length != 8));
			}
			break;

		case ENC_TIME_SECS_USECS|ENC_BIG_ENDIAN:
			/*
			 * 4-byte seconds, followed by 4-byte fractional
			 * time in microseconds, both big-endian.
			 * For absolute times, the seconds are seconds
			 * since the UN*X epoch.
			 */
			if (length == 8) {
				time_stamp->secs  = (time_t)tvb_get_ntohl(tvb, start);
				time_stamp->nsecs = tvb_get_ntohl(tvb, start+4)*1000;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a timeval", length, (length < 4));
			}
			break;

		case ENC_TIME_SECS_USECS|ENC_LITTLE_ENDIAN:
			/*
			 * 4-byte seconds, followed by 4-byte fractional
			 * time in microseconds, both little-endian.
			 * For absolute times, the seconds are seconds
			 * since the UN*X epoch.
			 */
			if (length == 8) {
				time_stamp->secs  = (time_t)tvb_get_letohl(tvb, start);
				time_stamp->nsecs = tvb_get_letohl(tvb, start+4)*1000;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a timeval", length, (length < 4));
			}
			break;

		case ENC_TIME_SECS|ENC_BIG_ENDIAN:
		case ENC_TIME_SECS|ENC_LITTLE_ENDIAN:
			/*
			 * Seconds, 1 to 8 bytes.
			 * For absolute times, it's seconds since the
			 * UN*X epoch.
			 */
			if (length >= 1 && length <= 8) {
				time_stamp->secs  = (time_t)get_uint64_value(tree, tvb, start, length, encoding);
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a time-in-seconds time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_MSECS|ENC_BIG_ENDIAN:
		case ENC_TIME_MSECS|ENC_LITTLE_ENDIAN:
			/*
			 * Milliseconds, 1 to 8 bytes.
			 * For absolute times, it's milliseconds since the
			 * UN*X epoch.
			 */
			if (length >= 1 && length <= 8) {
				guint64 msecs;

				msecs = get_uint64_value(tree, tvb, start, length, encoding);
				time_stamp->secs  = (time_t)(msecs / 1000);
				time_stamp->nsecs = (int)(msecs % 1000)*1000000;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a time-in-milliseconds time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_RFC_3971|ENC_BIG_ENDIAN:
			/*
			 * 1/64ths of a second since the UN*X epoch,
			 * big-endian.
			 *
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				/*
				 * The upper 48 bits are seconds since the
				 * UN*X epoch.
				 */
				time_stamp->secs  = (time_t)tvb_get_ntoh48(tvb, start);
				/*
				 * The lower 16 bits are 1/2^16s of a second;
				 * convert them to nanoseconds.
				 *
				 * XXX - this may give the impression of higher
				 * precision than you actually get.
				 */
				time_stamp->nsecs = (int)(1000000000*(tvb_get_ntohs(tvb, start+6)/65536.0));
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an RFC 3971-style time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_RFC_3971|ENC_LITTLE_ENDIAN:
			/*
			 * 1/64ths of a second since the UN*X epoch,
			 * little-endian.
			 *
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				/*
				 * XXX - this is assuming that, if anybody
				 * were ever to use this format - RFC 3971
				 * doesn't, because that's an Internet
				 * protocol, and those use network byte
				 * order, i.e. big-endian - they'd treat it
				 * as a 64-bit count of 1/2^16s of a second,
				 * putting the upper 48 bits at the end.
				 *
				 * The lower 48 bits are seconds since the
				 * UN*X epoch.
				 */
				time_stamp->secs  = (time_t)tvb_get_letoh48(tvb, start+2);
				/*
				 * The upper 16 bits are 1/2^16s of a second;
				 * convert them to nanoseconds.
				 *
				 * XXX - this may give the impression of higher
				 * precision than you actually get.
				 */
				time_stamp->nsecs = (int)(1000000000*(tvb_get_letohs(tvb, start)/65536.0));
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an RFC 3971-style time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_SECS_NTP|ENC_BIG_ENDIAN:
			/*
			 * NTP time stamp, with 1-second resolution (i.e.,
			 * seconds since the NTP epoch), big-endian.
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 4) {
				/*
				* We need a temporary variable here so the unsigned math
				* works correctly (for years > 2036 according to RFC 2030
				* chapter 3).
				*
				* If bit 0 is set, the UTC time is in the range 1968-2036 and
				* UTC time is reckoned from 0h 0m 0s UTC on 1 January 1900.
				* If bit 0 is not set, the time is in the range 2036-2104 and
				* UTC time is reckoned from 6h 28m 16s UTC on 7 February 2036.
				*/
				tmpsecs  = tvb_get_ntohl(tvb, start);
				if ((tmpsecs & 0x80000000) != 0)
					time_stamp->secs = (time_t)((gint64)tmpsecs - NTP_TIMEDIFF1900TO1970SEC);
				else
					time_stamp->secs = (time_t)((gint64)tmpsecs + NTP_TIMEDIFF1970TO2036SEC);
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an NTP seconds-only time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_SECS_NTP|ENC_LITTLE_ENDIAN:
			/*
			 * NTP time stamp, with 1-second resolution (i.e.,
			 * seconds since the NTP epoch), little-endian.
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			/*
			 * We need a temporary variable here so the unsigned math
			 * works correctly (for years > 2036 according to RFC 2030
			 * chapter 3).
			 *
			 * If bit 0 is set, the UTC time is in the range 1968-2036 and
			 * UTC time is reckoned from 0h 0m 0s UTC on 1 January 1900.
			 * If bit 0 is not set, the time is in the range 2036-2104 and
			 * UTC time is reckoned from 6h 28m 16s UTC on 7 February 2036.
			 */
			if (length == 4) {
				tmpsecs  = tvb_get_letohl(tvb, start);
				if ((tmpsecs & 0x80000000) != 0)
					time_stamp->secs = (time_t)((gint64)tmpsecs - NTP_TIMEDIFF1900TO1970SEC);
				else
					time_stamp->secs = (time_t)((gint64)tmpsecs + NTP_TIMEDIFF1970TO2036SEC);
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an NTP seconds-only time stamp", length, (length < 4));
			}
			break;
		case ENC_TIME_MSEC_NTP | ENC_BIG_ENDIAN:
			/*
			* Milliseconds, 1 to 8 bytes.
			* For absolute times, it's milliseconds since the
			* NTP epoch.
			*/
			if (length >= 1 && length <= 8) {
				guint64 msecs;

				msecs = get_uint64_value(tree, tvb, start, length, encoding);
				tmpsecs = (guint32)(msecs / 1000);
				/*
				* If bit 0 is set, the UTC time is in the range 1968-2036 and
				* UTC time is reckoned from 0h 0m 0s UTC on 1 January 1900.
				* If bit 0 is not set, the time is in the range 2036-2104 and
				* UTC time is reckoned from 6h 28m 16s UTC on 7 February 2036.
				*/
				if ((tmpsecs & 0x80000000) != 0)
					time_stamp->secs = (time_t)((gint64)tmpsecs - NTP_TIMEDIFF1900TO1970SEC);
				else
					time_stamp->secs = (time_t)((gint64)tmpsecs + NTP_TIMEDIFF1970TO2036SEC);
				time_stamp->nsecs = (int)(msecs % 1000)*1000000;
			}
			else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "a time-in-milliseconds NTP time stamp", length, (length < 4));
			}
			break;

		case ENC_TIME_CLASSIC_MAC_OS_SECS|ENC_BIG_ENDIAN:
			/*
			 * Classic Mac OS time stamps, big-endian.
			 * Only supported for absolute times.
			 */
			DISSECTOR_ASSERT(!is_relative);

			if (length == 8) {
				tmp64secs  = tvb_get_ntoh64(tvb, start);
				time_stamp->secs = (time_t)(gint64)(tmp64secs - EPOCH_DELTA_1904_01_01_00_00_00_UTC);
				time_stamp->nsecs = 0;
			} else if (length == 4) {
				tmpsecs  = tvb_get_ntohl(tvb, start);
				time_stamp->secs = (time_t)(gint32)(tmpsecs - EPOCH_DELTA_1904_01_01_00_00_00_UTC);
				time_stamp->nsecs = 0;
			} else {
				time_stamp->secs  = 0;
				time_stamp->nsecs = 0;
				report_type_length_mismatch(tree, "an MP4 time stamp", length, (length < 4));
			}
			break;

		default:
			DISSECTOR_ASSERT_NOT_REACHED();
			break;
	}
}