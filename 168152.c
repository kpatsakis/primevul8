networkstatus_compute_consensus(smartlist_t *votes,
                                int total_authorities,
                                crypto_pk_t *identity_key,
                                crypto_pk_t *signing_key,
                                const char *legacy_id_key_digest,
                                crypto_pk_t *legacy_signing_key,
                                consensus_flavor_t flavor)
{
  smartlist_t *chunks;
  char *result = NULL;
  int consensus_method;
  time_t valid_after, fresh_until, valid_until;
  int vote_seconds, dist_seconds;
  char *client_versions = NULL, *server_versions = NULL;
  smartlist_t *flags;
  const char *flavor_name;
  uint32_t max_unmeasured_bw_kb = DEFAULT_MAX_UNMEASURED_BW_KB;
  int64_t G=0, M=0, E=0, D=0, T=0; /* For bandwidth weights */
  const routerstatus_format_type_t rs_format =
    flavor == FLAV_NS ? NS_V3_CONSENSUS : NS_V3_CONSENSUS_MICRODESC;
  char *params = NULL;
  char *packages = NULL;
  int added_weights = 0;
  dircollator_t *collator = NULL;
  smartlist_t *param_list = NULL;

  tor_assert(flavor == FLAV_NS || flavor == FLAV_MICRODESC);
  tor_assert(total_authorities >= smartlist_len(votes));
  tor_assert(total_authorities > 0);

  flavor_name = networkstatus_get_flavor_name(flavor);

  if (!smartlist_len(votes)) {
    log_warn(LD_DIR, "Can't compute a consensus from no votes.");
    return NULL;
  }
  flags = smartlist_new();

  consensus_method = compute_consensus_method(votes);
  if (consensus_method_is_supported(consensus_method)) {
    log_info(LD_DIR, "Generating consensus using method %d.",
             consensus_method);
  } else {
    log_warn(LD_DIR, "The other authorities will use consensus method %d, "
             "which I don't support.  Maybe I should upgrade!",
             consensus_method);
    consensus_method = MAX_SUPPORTED_CONSENSUS_METHOD;
  }

  /* Compute medians of time-related things, and figure out how many
   * routers we might need to talk about. */
  {
    int n_votes = smartlist_len(votes);
    time_t *va_times = tor_calloc(n_votes, sizeof(time_t));
    time_t *fu_times = tor_calloc(n_votes, sizeof(time_t));
    time_t *vu_times = tor_calloc(n_votes, sizeof(time_t));
    int *votesec_list = tor_calloc(n_votes, sizeof(int));
    int *distsec_list = tor_calloc(n_votes, sizeof(int));
    int n_versioning_clients = 0, n_versioning_servers = 0;
    smartlist_t *combined_client_versions = smartlist_new();
    smartlist_t *combined_server_versions = smartlist_new();

    SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
      tor_assert(v->type == NS_TYPE_VOTE);
      va_times[v_sl_idx] = v->valid_after;
      fu_times[v_sl_idx] = v->fresh_until;
      vu_times[v_sl_idx] = v->valid_until;
      votesec_list[v_sl_idx] = v->vote_seconds;
      distsec_list[v_sl_idx] = v->dist_seconds;
      if (v->client_versions) {
        smartlist_t *cv = smartlist_new();
        ++n_versioning_clients;
        smartlist_split_string(cv, v->client_versions, ",",
                               SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
        sort_version_list(cv, 1);
        smartlist_add_all(combined_client_versions, cv);
        smartlist_free(cv); /* elements get freed later. */
      }
      if (v->server_versions) {
        smartlist_t *sv = smartlist_new();
        ++n_versioning_servers;
        smartlist_split_string(sv, v->server_versions, ",",
                               SPLIT_SKIP_SPACE|SPLIT_IGNORE_BLANK, 0);
        sort_version_list(sv, 1);
        smartlist_add_all(combined_server_versions, sv);
        smartlist_free(sv); /* elements get freed later. */
      }
      SMARTLIST_FOREACH(v->known_flags, const char *, cp,
                        smartlist_add(flags, tor_strdup(cp)));
    } SMARTLIST_FOREACH_END(v);
    valid_after = median_time(va_times, n_votes);
    fresh_until = median_time(fu_times, n_votes);
    valid_until = median_time(vu_times, n_votes);
    vote_seconds = median_int(votesec_list, n_votes);
    dist_seconds = median_int(distsec_list, n_votes);

    tor_assert(valid_after +
               (get_options()->TestingTorNetwork ?
                MIN_VOTE_INTERVAL_TESTING : MIN_VOTE_INTERVAL) <= fresh_until);
    tor_assert(fresh_until +
               (get_options()->TestingTorNetwork ?
                MIN_VOTE_INTERVAL_TESTING : MIN_VOTE_INTERVAL) <= valid_until);
    tor_assert(vote_seconds >= MIN_VOTE_SECONDS);
    tor_assert(dist_seconds >= MIN_DIST_SECONDS);

    server_versions = compute_consensus_versions_list(combined_server_versions,
                                                      n_versioning_servers);
    client_versions = compute_consensus_versions_list(combined_client_versions,
                                                      n_versioning_clients);
    if (consensus_method >= MIN_METHOD_FOR_PACKAGE_LINES) {
      packages = compute_consensus_package_lines(votes);
    } else {
      packages = tor_strdup("");
    }

    SMARTLIST_FOREACH(combined_server_versions, char *, cp, tor_free(cp));
    SMARTLIST_FOREACH(combined_client_versions, char *, cp, tor_free(cp));
    smartlist_free(combined_server_versions);
    smartlist_free(combined_client_versions);

    if (consensus_method >= MIN_METHOD_FOR_ED25519_ID_VOTING)
      smartlist_add(flags, tor_strdup("NoEdConsensus"));

    smartlist_sort_strings(flags);
    smartlist_uniq_strings(flags);

    tor_free(va_times);
    tor_free(fu_times);
    tor_free(vu_times);
    tor_free(votesec_list);
    tor_free(distsec_list);
  }

  chunks = smartlist_new();

  {
    char va_buf[ISO_TIME_LEN+1], fu_buf[ISO_TIME_LEN+1],
      vu_buf[ISO_TIME_LEN+1];
    char *flaglist;
    format_iso_time(va_buf, valid_after);
    format_iso_time(fu_buf, fresh_until);
    format_iso_time(vu_buf, valid_until);
    flaglist = smartlist_join_strings(flags, " ", 0, NULL);

    smartlist_add_asprintf(chunks, "network-status-version 3%s%s\n"
                 "vote-status consensus\n",
                 flavor == FLAV_NS ? "" : " ",
                 flavor == FLAV_NS ? "" : flavor_name);

    smartlist_add_asprintf(chunks, "consensus-method %d\n",
                           consensus_method);

    smartlist_add_asprintf(chunks,
                 "valid-after %s\n"
                 "fresh-until %s\n"
                 "valid-until %s\n"
                 "voting-delay %d %d\n"
                 "client-versions %s\n"
                 "server-versions %s\n"
                 "%s" /* packages */
                 "known-flags %s\n",
                 va_buf, fu_buf, vu_buf,
                 vote_seconds, dist_seconds,
                 client_versions, server_versions,
                 packages,
                 flaglist);

    tor_free(flaglist);
  }

  if (consensus_method >= MIN_METHOD_FOR_RECOMMENDED_PROTOCOLS) {
    int num_dirauth = get_n_authorities(V3_DIRINFO);
    int idx;
    for (idx = 0; idx < 4; ++idx) {
      char *proto_line = compute_nth_protocol_set(idx, num_dirauth, votes);
      if (BUG(!proto_line))
        continue;
      smartlist_add(chunks, proto_line);
    }
  }

  param_list = dirvote_compute_params(votes, consensus_method,
                                      total_authorities);
  if (smartlist_len(param_list)) {
    params = smartlist_join_strings(param_list, " ", 0, NULL);
    smartlist_add(chunks, tor_strdup("params "));
    smartlist_add(chunks, params);
    smartlist_add(chunks, tor_strdup("\n"));
  }

  if (consensus_method >= MIN_METHOD_FOR_SHARED_RANDOM) {
    int num_dirauth = get_n_authorities(V3_DIRINFO);
    /* Default value of this is 2/3 of the total number of authorities. For
     * instance, if we have 9 dirauth, the default value is 6. The following
     * calculation will round it down. */
    int32_t num_srv_agreements =
      dirvote_get_intermediate_param_value(param_list,
                                           "AuthDirNumSRVAgreements",
                                           (num_dirauth * 2) / 3);
    /* Add the shared random value. */
    char *srv_lines = sr_get_string_for_consensus(votes, num_srv_agreements);
    if (srv_lines != NULL) {
      smartlist_add(chunks, srv_lines);
    }
  }

  /* Sort the votes. */
  smartlist_sort(votes, compare_votes_by_authority_id_);
  /* Add the authority sections. */
  {
    smartlist_t *dir_sources = smartlist_new();
    SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
      dir_src_ent_t *e = tor_malloc_zero(sizeof(dir_src_ent_t));
      e->v = v;
      e->digest = get_voter(v)->identity_digest;
      e->is_legacy = 0;
      smartlist_add(dir_sources, e);
      if (!tor_digest_is_zero(get_voter(v)->legacy_id_digest)) {
        dir_src_ent_t *e_legacy = tor_malloc_zero(sizeof(dir_src_ent_t));
        e_legacy->v = v;
        e_legacy->digest = get_voter(v)->legacy_id_digest;
        e_legacy->is_legacy = 1;
        smartlist_add(dir_sources, e_legacy);
      }
    } SMARTLIST_FOREACH_END(v);
    smartlist_sort(dir_sources, compare_dir_src_ents_by_authority_id_);

    SMARTLIST_FOREACH_BEGIN(dir_sources, const dir_src_ent_t *, e) {
      char fingerprint[HEX_DIGEST_LEN+1];
      char votedigest[HEX_DIGEST_LEN+1];
      networkstatus_t *v = e->v;
      networkstatus_voter_info_t *voter = get_voter(v);

      base16_encode(fingerprint, sizeof(fingerprint), e->digest, DIGEST_LEN);
      base16_encode(votedigest, sizeof(votedigest), voter->vote_digest,
                    DIGEST_LEN);

      smartlist_add_asprintf(chunks,
                   "dir-source %s%s %s %s %s %d %d\n",
                   voter->nickname, e->is_legacy ? "-legacy" : "",
                   fingerprint, voter->address, fmt_addr32(voter->addr),
                   voter->dir_port,
                   voter->or_port);
      if (! e->is_legacy) {
        smartlist_add_asprintf(chunks,
                     "contact %s\n"
                     "vote-digest %s\n",
                     voter->contact,
                     votedigest);
      }
    } SMARTLIST_FOREACH_END(e);
    SMARTLIST_FOREACH(dir_sources, dir_src_ent_t *, e, tor_free(e));
    smartlist_free(dir_sources);
  }

  if (consensus_method >= MIN_METHOD_TO_CLIP_UNMEASURED_BW) {
    char *max_unmeasured_param = NULL;
    /* XXXX Extract this code into a common function.  Or don't!  see #19011 */
    if (params) {
      if (strcmpstart(params, "maxunmeasuredbw=") == 0)
        max_unmeasured_param = params;
      else
        max_unmeasured_param = strstr(params, " maxunmeasuredbw=");
    }
    if (max_unmeasured_param) {
      int ok = 0;
      char *eq = strchr(max_unmeasured_param, '=');
      if (eq) {
        max_unmeasured_bw_kb = (uint32_t)
          tor_parse_ulong(eq+1, 10, 1, UINT32_MAX, &ok, NULL);
        if (!ok) {
          log_warn(LD_DIR, "Bad element '%s' in max unmeasured bw param",
                   escaped(max_unmeasured_param));
          max_unmeasured_bw_kb = DEFAULT_MAX_UNMEASURED_BW_KB;
        }
      }
    }
  }

  /* Add the actual router entries. */
  {
    int *size; /* size[j] is the number of routerstatuses in votes[j]. */
    int *flag_counts; /* The number of voters that list flag[j] for the
                       * currently considered router. */
    int i;
    smartlist_t *matching_descs = smartlist_new();
    smartlist_t *chosen_flags = smartlist_new();
    smartlist_t *versions = smartlist_new();
    smartlist_t *protocols = smartlist_new();
    smartlist_t *exitsummaries = smartlist_new();
    uint32_t *bandwidths_kb = tor_calloc(smartlist_len(votes),
                                         sizeof(uint32_t));
    uint32_t *measured_bws_kb = tor_calloc(smartlist_len(votes),
                                           sizeof(uint32_t));
    uint32_t *measured_guardfraction = tor_calloc(smartlist_len(votes),
                                                  sizeof(uint32_t));
    int num_bandwidths;
    int num_mbws;
    int num_guardfraction_inputs;

    int *n_voter_flags; /* n_voter_flags[j] is the number of flags that
                         * votes[j] knows about. */
    int *n_flag_voters; /* n_flag_voters[f] is the number of votes that care
                         * about flags[f]. */
    int **flag_map; /* flag_map[j][b] is an index f such that flag_map[f]
                     * is the same flag as votes[j]->known_flags[b]. */
    int *named_flag; /* Index of the flag "Named" for votes[j] */
    int *unnamed_flag; /* Index of the flag "Unnamed" for votes[j] */
    int n_authorities_measuring_bandwidth;

    strmap_t *name_to_id_map = strmap_new();
    char conflict[DIGEST_LEN];
    char unknown[DIGEST_LEN];
    memset(conflict, 0, sizeof(conflict));
    memset(unknown, 0xff, sizeof(conflict));

    size = tor_calloc(smartlist_len(votes), sizeof(int));
    n_voter_flags = tor_calloc(smartlist_len(votes), sizeof(int));
    n_flag_voters = tor_calloc(smartlist_len(flags), sizeof(int));
    flag_map = tor_calloc(smartlist_len(votes), sizeof(int *));
    named_flag = tor_calloc(smartlist_len(votes), sizeof(int));
    unnamed_flag = tor_calloc(smartlist_len(votes), sizeof(int));
    for (i = 0; i < smartlist_len(votes); ++i)
      unnamed_flag[i] = named_flag[i] = -1;

    /* Build the flag indexes. Note that no vote can have more than 64 members
     * for known_flags, so no value will be greater than 63, so it's safe to
     * do U64_LITERAL(1) << index on these values.  But note also that
     * named_flag and unnamed_flag are initialized to -1, so we need to check
     * that they're actually set before doing U64_LITERAL(1) << index with
     * them.*/
    SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
      flag_map[v_sl_idx] = tor_calloc(smartlist_len(v->known_flags),
                                      sizeof(int));
      if (smartlist_len(v->known_flags) > MAX_KNOWN_FLAGS_IN_VOTE) {
        log_warn(LD_BUG, "Somehow, a vote has %d entries in known_flags",
                 smartlist_len(v->known_flags));
      }
      SMARTLIST_FOREACH_BEGIN(v->known_flags, const char *, fl) {
        int p = smartlist_string_pos(flags, fl);
        tor_assert(p >= 0);
        flag_map[v_sl_idx][fl_sl_idx] = p;
        ++n_flag_voters[p];
        if (!strcmp(fl, "Named"))
          named_flag[v_sl_idx] = fl_sl_idx;
        if (!strcmp(fl, "Unnamed"))
          unnamed_flag[v_sl_idx] = fl_sl_idx;
      } SMARTLIST_FOREACH_END(fl);
      n_voter_flags[v_sl_idx] = smartlist_len(v->known_flags);
      size[v_sl_idx] = smartlist_len(v->routerstatus_list);
    } SMARTLIST_FOREACH_END(v);

    /* Named and Unnamed get treated specially */
    {
      SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
        uint64_t nf;
        if (named_flag[v_sl_idx]<0)
          continue;
        nf = U64_LITERAL(1) << named_flag[v_sl_idx];
        SMARTLIST_FOREACH_BEGIN(v->routerstatus_list,
                                vote_routerstatus_t *, rs) {

          if ((rs->flags & nf) != 0) {
            const char *d = strmap_get_lc(name_to_id_map, rs->status.nickname);
            if (!d) {
              /* We have no name officially mapped to this digest. */
              strmap_set_lc(name_to_id_map, rs->status.nickname,
                            rs->status.identity_digest);
            } else if (d != conflict &&
                fast_memcmp(d, rs->status.identity_digest, DIGEST_LEN)) {
              /* Authorities disagree about this nickname. */
              strmap_set_lc(name_to_id_map, rs->status.nickname, conflict);
            } else {
              /* It's already a conflict, or it's already this ID. */
            }
          }
        } SMARTLIST_FOREACH_END(rs);
      } SMARTLIST_FOREACH_END(v);

      SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
        uint64_t uf;
        if (unnamed_flag[v_sl_idx]<0)
          continue;
        uf = U64_LITERAL(1) << unnamed_flag[v_sl_idx];
        SMARTLIST_FOREACH_BEGIN(v->routerstatus_list,
                                vote_routerstatus_t *, rs) {
          if ((rs->flags & uf) != 0) {
            const char *d = strmap_get_lc(name_to_id_map, rs->status.nickname);
            if (d == conflict || d == unknown) {
              /* Leave it alone; we know what it is. */
            } else if (!d) {
              /* We have no name officially mapped to this digest. */
              strmap_set_lc(name_to_id_map, rs->status.nickname, unknown);
            } else if (fast_memeq(d, rs->status.identity_digest, DIGEST_LEN)) {
              /* Authorities disagree about this nickname. */
              strmap_set_lc(name_to_id_map, rs->status.nickname, conflict);
            } else {
              /* It's mapped to a different name. */
            }
          }
        } SMARTLIST_FOREACH_END(rs);
      } SMARTLIST_FOREACH_END(v);
    }

    /* We need to know how many votes measure bandwidth. */
    n_authorities_measuring_bandwidth = 0;
    SMARTLIST_FOREACH(votes, const networkstatus_t *, v,
       if (v->has_measured_bws) {
         ++n_authorities_measuring_bandwidth;
       }
    );

    /* Populate the collator */
    collator = dircollator_new(smartlist_len(votes), total_authorities);
    SMARTLIST_FOREACH_BEGIN(votes, networkstatus_t *, v) {
      dircollator_add_vote(collator, v);
    } SMARTLIST_FOREACH_END(v);

    dircollator_collate(collator, consensus_method);

    /* Now go through all the votes */
    flag_counts = tor_calloc(smartlist_len(flags), sizeof(int));
    const int num_routers = dircollator_n_routers(collator);
    for (i = 0; i < num_routers; ++i) {
      vote_routerstatus_t **vrs_lst =
        dircollator_get_votes_for_router(collator, i);

      vote_routerstatus_t *rs;
      routerstatus_t rs_out;
      const char *current_rsa_id = NULL;
      const char *chosen_version;
      const char *chosen_protocol_list;
      const char *chosen_name = NULL;
      int exitsummary_disagreement = 0;
      int is_named = 0, is_unnamed = 0, is_running = 0, is_valid = 0;
      int is_guard = 0, is_exit = 0, is_bad_exit = 0;
      int naming_conflict = 0;
      int n_listing = 0;
      char microdesc_digest[DIGEST256_LEN];
      tor_addr_port_t alt_orport = {TOR_ADDR_NULL, 0};

      memset(flag_counts, 0, sizeof(int)*smartlist_len(flags));
      smartlist_clear(matching_descs);
      smartlist_clear(chosen_flags);
      smartlist_clear(versions);
      smartlist_clear(protocols);
      num_bandwidths = 0;
      num_mbws = 0;
      num_guardfraction_inputs = 0;
      int ed_consensus = 0;
      const uint8_t *ed_consensus_val = NULL;

      /* Okay, go through all the entries for this digest. */
      for (int voter_idx = 0; voter_idx < smartlist_len(votes); ++voter_idx) {
        if (vrs_lst[voter_idx] == NULL)
          continue; /* This voter had nothing to say about this entry. */
        rs = vrs_lst[voter_idx];
        ++n_listing;

        current_rsa_id = rs->status.identity_digest;

        smartlist_add(matching_descs, rs);
        if (rs->version && rs->version[0])
          smartlist_add(versions, rs->version);

        if (rs->protocols) {
          /* We include this one even if it's empty: voting for an
           * empty protocol list actually is meaningful. */
          smartlist_add(protocols, rs->protocols);
        }

        /* Tally up all the flags. */
        for (int flag = 0; flag < n_voter_flags[voter_idx]; ++flag) {
          if (rs->flags & (U64_LITERAL(1) << flag))
            ++flag_counts[flag_map[voter_idx][flag]];
        }
        if (named_flag[voter_idx] >= 0 &&
            (rs->flags & (U64_LITERAL(1) << named_flag[voter_idx]))) {
          if (chosen_name && strcmp(chosen_name, rs->status.nickname)) {
            log_notice(LD_DIR, "Conflict on naming for router: %s vs %s",
                       chosen_name, rs->status.nickname);
            naming_conflict = 1;
          }
          chosen_name = rs->status.nickname;
        }

        /* Count guardfraction votes and note down the values. */
        if (rs->status.has_guardfraction) {
          measured_guardfraction[num_guardfraction_inputs++] =
            rs->status.guardfraction_percentage;
        }

        /* count bandwidths */
        if (rs->has_measured_bw)
          measured_bws_kb[num_mbws++] = rs->measured_bw_kb;

        if (rs->status.has_bandwidth)
          bandwidths_kb[num_bandwidths++] = rs->status.bandwidth_kb;

        /* Count number for which ed25519 is canonical. */
        if (rs->ed25519_reflects_consensus) {
          ++ed_consensus;
          if (ed_consensus_val) {
            tor_assert(fast_memeq(ed_consensus_val, rs->ed25519_id,
                                  ED25519_PUBKEY_LEN));
          } else {
            ed_consensus_val = rs->ed25519_id;
          }
        }
      }

      /* We don't include this router at all unless more than half of
       * the authorities we believe in list it. */
      if (n_listing <= total_authorities/2)
        continue;

      if (ed_consensus > 0) {
        tor_assert(consensus_method >= MIN_METHOD_FOR_ED25519_ID_VOTING);
        if (ed_consensus <= total_authorities / 2) {
          log_warn(LD_BUG, "Not enough entries had ed_consensus set; how "
                   "can we have a consensus of %d?", ed_consensus);
        }
      }

      /* The clangalyzer can't figure out that this will never be NULL
       * if n_listing is at least 1 */
      tor_assert(current_rsa_id);

      /* Figure out the most popular opinion of what the most recent
       * routerinfo and its contents are. */
      memset(microdesc_digest, 0, sizeof(microdesc_digest));
      rs = compute_routerstatus_consensus(matching_descs, consensus_method,
                                          microdesc_digest, &alt_orport);
      /* Copy bits of that into rs_out. */
      memset(&rs_out, 0, sizeof(rs_out));
      tor_assert(fast_memeq(current_rsa_id,
                            rs->status.identity_digest,DIGEST_LEN));
      memcpy(rs_out.identity_digest, current_rsa_id, DIGEST_LEN);
      memcpy(rs_out.descriptor_digest, rs->status.descriptor_digest,
             DIGEST_LEN);
      rs_out.addr = rs->status.addr;
      rs_out.published_on = rs->status.published_on;
      rs_out.dir_port = rs->status.dir_port;
      rs_out.or_port = rs->status.or_port;
      if (consensus_method >= MIN_METHOD_FOR_A_LINES) {
        tor_addr_copy(&rs_out.ipv6_addr, &alt_orport.addr);
        rs_out.ipv6_orport = alt_orport.port;
      }
      rs_out.has_bandwidth = 0;
      rs_out.has_exitsummary = 0;

      if (chosen_name && !naming_conflict) {
        strlcpy(rs_out.nickname, chosen_name, sizeof(rs_out.nickname));
      } else {
        strlcpy(rs_out.nickname, rs->status.nickname, sizeof(rs_out.nickname));
      }

      {
        const char *d = strmap_get_lc(name_to_id_map, rs_out.nickname);
        if (!d) {
          is_named = is_unnamed = 0;
        } else if (fast_memeq(d, current_rsa_id, DIGEST_LEN)) {
          is_named = 1; is_unnamed = 0;
        } else {
          is_named = 0; is_unnamed = 1;
        }
      }

      /* Set the flags. */
      smartlist_add(chosen_flags, (char*)"s"); /* for the start of the line. */
      SMARTLIST_FOREACH_BEGIN(flags, const char *, fl) {
        if (!strcmp(fl, "Named")) {
          if (is_named)
            smartlist_add(chosen_flags, (char*)fl);
        } else if (!strcmp(fl, "Unnamed")) {
          if (is_unnamed)
            smartlist_add(chosen_flags, (char*)fl);
        } else if (!strcmp(fl, "NoEdConsensus") &&
                   consensus_method >= MIN_METHOD_FOR_ED25519_ID_VOTING) {
          if (ed_consensus <= total_authorities/2)
            smartlist_add(chosen_flags, (char*)fl);
        } else {
          if (flag_counts[fl_sl_idx] > n_flag_voters[fl_sl_idx]/2) {
            smartlist_add(chosen_flags, (char*)fl);
            if (!strcmp(fl, "Exit"))
              is_exit = 1;
            else if (!strcmp(fl, "Guard"))
              is_guard = 1;
            else if (!strcmp(fl, "Running"))
              is_running = 1;
            else if (!strcmp(fl, "BadExit"))
              is_bad_exit = 1;
            else if (!strcmp(fl, "Valid"))
              is_valid = 1;
          }
        }
      } SMARTLIST_FOREACH_END(fl);

      /* Starting with consensus method 4 we do not list servers
       * that are not running in a consensus.  See Proposal 138 */
      if (!is_running)
        continue;

      /* Starting with consensus method 24, we don't list servers
       * that are not valid in a consensus.  See Proposal 272 */
      if (!is_valid &&
          consensus_method >= MIN_METHOD_FOR_EXCLUDING_INVALID_NODES)
        continue;

      /* Pick the version. */
      if (smartlist_len(versions)) {
        sort_version_list(versions, 0);
        chosen_version = get_most_frequent_member(versions);
      } else {
        chosen_version = NULL;
      }

      /* Pick the protocol list */
      if (smartlist_len(protocols)) {
        smartlist_sort_strings(protocols);
        chosen_protocol_list = get_most_frequent_member(protocols);
      } else {
        chosen_protocol_list = NULL;
      }

      /* If it's a guard and we have enough guardfraction votes,
         calculate its consensus guardfraction value. */
      if (is_guard && num_guardfraction_inputs > 2 &&
          consensus_method >= MIN_METHOD_FOR_GUARDFRACTION) {
        rs_out.has_guardfraction = 1;
        rs_out.guardfraction_percentage = median_uint32(measured_guardfraction,
                                                     num_guardfraction_inputs);
        /* final value should be an integer percentage! */
        tor_assert(rs_out.guardfraction_percentage <= 100);
      }

      /* Pick a bandwidth */
      if (num_mbws > 2) {
        rs_out.has_bandwidth = 1;
        rs_out.bw_is_unmeasured = 0;
        rs_out.bandwidth_kb = median_uint32(measured_bws_kb, num_mbws);
      } else if (num_bandwidths > 0) {
        rs_out.has_bandwidth = 1;
        rs_out.bw_is_unmeasured = 1;
        rs_out.bandwidth_kb = median_uint32(bandwidths_kb, num_bandwidths);
        if (consensus_method >= MIN_METHOD_TO_CLIP_UNMEASURED_BW &&
            n_authorities_measuring_bandwidth > 2) {
          /* Cap non-measured bandwidths. */
          if (rs_out.bandwidth_kb > max_unmeasured_bw_kb) {
            rs_out.bandwidth_kb = max_unmeasured_bw_kb;
          }
        }
      }

      /* Fix bug 2203: Do not count BadExit nodes as Exits for bw weights */
      is_exit = is_exit && !is_bad_exit;

      /* Update total bandwidth weights with the bandwidths of this router. */
      {
        update_total_bandwidth_weights(&rs_out,
                                       is_exit, is_guard,
                                       &G, &M, &E, &D, &T);
      }

      /* Ok, we already picked a descriptor digest we want to list
       * previously.  Now we want to use the exit policy summary from
       * that descriptor.  If everybody plays nice all the voters who
       * listed that descriptor will have the same summary.  If not then
       * something is fishy and we'll use the most common one (breaking
       * ties in favor of lexicographically larger one (only because it
       * lets me reuse more existing code)).
       *
       * The other case that can happen is that no authority that voted
       * for that descriptor has an exit policy summary.  That's
       * probably quite unlikely but can happen.  In that case we use
       * the policy that was most often listed in votes, again breaking
       * ties like in the previous case.
       */
      {
        /* Okay, go through all the votes for this router.  We prepared
         * that list previously */
        const char *chosen_exitsummary = NULL;
        smartlist_clear(exitsummaries);
        SMARTLIST_FOREACH_BEGIN(matching_descs, vote_routerstatus_t *, vsr) {
          /* Check if the vote where this status comes from had the
           * proper descriptor */
          tor_assert(fast_memeq(rs_out.identity_digest,
                             vsr->status.identity_digest,
                             DIGEST_LEN));
          if (vsr->status.has_exitsummary &&
               fast_memeq(rs_out.descriptor_digest,
                       vsr->status.descriptor_digest,
                       DIGEST_LEN)) {
            tor_assert(vsr->status.exitsummary);
            smartlist_add(exitsummaries, vsr->status.exitsummary);
            if (!chosen_exitsummary) {
              chosen_exitsummary = vsr->status.exitsummary;
            } else if (strcmp(chosen_exitsummary, vsr->status.exitsummary)) {
              /* Great.  There's disagreement among the voters.  That
               * really shouldn't be */
              exitsummary_disagreement = 1;
            }
          }
        } SMARTLIST_FOREACH_END(vsr);

        if (exitsummary_disagreement) {
          char id[HEX_DIGEST_LEN+1];
          char dd[HEX_DIGEST_LEN+1];
          base16_encode(id, sizeof(dd), rs_out.identity_digest, DIGEST_LEN);
          base16_encode(dd, sizeof(dd), rs_out.descriptor_digest, DIGEST_LEN);
          log_warn(LD_DIR, "The voters disagreed on the exit policy summary "
                   " for router %s with descriptor %s.  This really shouldn't"
                   " have happened.", id, dd);

          smartlist_sort_strings(exitsummaries);
          chosen_exitsummary = get_most_frequent_member(exitsummaries);
        } else if (!chosen_exitsummary) {
          char id[HEX_DIGEST_LEN+1];
          char dd[HEX_DIGEST_LEN+1];
          base16_encode(id, sizeof(dd), rs_out.identity_digest, DIGEST_LEN);
          base16_encode(dd, sizeof(dd), rs_out.descriptor_digest, DIGEST_LEN);
          log_warn(LD_DIR, "Not one of the voters that made us select"
                   "descriptor %s for router %s had an exit policy"
                   "summary", dd, id);

          /* Ok, none of those voting for the digest we chose had an
           * exit policy for us.  Well, that kinda sucks.
           */
          smartlist_clear(exitsummaries);
          SMARTLIST_FOREACH(matching_descs, vote_routerstatus_t *, vsr, {
            if (vsr->status.has_exitsummary)
              smartlist_add(exitsummaries, vsr->status.exitsummary);
          });
          smartlist_sort_strings(exitsummaries);
          chosen_exitsummary = get_most_frequent_member(exitsummaries);

          if (!chosen_exitsummary)
            log_warn(LD_DIR, "Wow, not one of the voters had an exit "
                     "policy summary for %s.  Wow.", id);
        }

        if (chosen_exitsummary) {
          rs_out.has_exitsummary = 1;
          /* yea, discards the const */
          rs_out.exitsummary = (char *)chosen_exitsummary;
        }
      }

      if (flavor == FLAV_MICRODESC &&
          tor_digest256_is_zero(microdesc_digest)) {
        /* With no microdescriptor digest, we omit the entry entirely. */
        continue;
      }

      {
        char *buf;
        /* Okay!! Now we can write the descriptor... */
        /*     First line goes into "buf". */
        buf = routerstatus_format_entry(&rs_out, NULL, NULL, rs_format, NULL);
        if (buf)
          smartlist_add(chunks, buf);
      }
      /*     Now an m line, if applicable. */
      if (flavor == FLAV_MICRODESC &&
          !tor_digest256_is_zero(microdesc_digest)) {
        char m[BASE64_DIGEST256_LEN+1];
        digest256_to_base64(m, microdesc_digest);
        smartlist_add_asprintf(chunks, "m %s\n", m);
      }
      /*     Next line is all flags.  The "\n" is missing. */
      smartlist_add(chunks,
                    smartlist_join_strings(chosen_flags, " ", 0, NULL));
      /*     Now the version line. */
      if (chosen_version) {
        smartlist_add(chunks, tor_strdup("\nv "));
        smartlist_add(chunks, tor_strdup(chosen_version));
      }
      smartlist_add(chunks, tor_strdup("\n"));
      if (chosen_protocol_list &&
          consensus_method >= MIN_METHOD_FOR_RS_PROTOCOLS) {
        smartlist_add_asprintf(chunks, "pr %s\n", chosen_protocol_list);
      }
      /*     Now the weight line. */
      if (rs_out.has_bandwidth) {
        char *guardfraction_str = NULL;
        int unmeasured = rs_out.bw_is_unmeasured &&
          consensus_method >= MIN_METHOD_TO_CLIP_UNMEASURED_BW;

        /* If we have guardfraction info, include it in the 'w' line. */
        if (rs_out.has_guardfraction) {
          tor_asprintf(&guardfraction_str,
                       " GuardFraction=%u", rs_out.guardfraction_percentage);
        }
        smartlist_add_asprintf(chunks, "w Bandwidth=%d%s%s\n",
                               rs_out.bandwidth_kb,
                               unmeasured?" Unmeasured=1":"",
                               guardfraction_str ? guardfraction_str : "");

        tor_free(guardfraction_str);
      }

      /*     Now the exitpolicy summary line. */
      if (rs_out.has_exitsummary && flavor == FLAV_NS) {
        smartlist_add_asprintf(chunks, "p %s\n", rs_out.exitsummary);
      }

      /* And the loop is over and we move on to the next router */
    }

    tor_free(size);
    tor_free(n_voter_flags);
    tor_free(n_flag_voters);
    for (i = 0; i < smartlist_len(votes); ++i)
      tor_free(flag_map[i]);
    tor_free(flag_map);
    tor_free(flag_counts);
    tor_free(named_flag);
    tor_free(unnamed_flag);
    strmap_free(name_to_id_map, NULL);
    smartlist_free(matching_descs);
    smartlist_free(chosen_flags);
    smartlist_free(versions);
    smartlist_free(protocols);
    smartlist_free(exitsummaries);
    tor_free(bandwidths_kb);
    tor_free(measured_bws_kb);
    tor_free(measured_guardfraction);
  }

  /* Mark the directory footer region */
  smartlist_add(chunks, tor_strdup("directory-footer\n"));

  {
    int64_t weight_scale = BW_WEIGHT_SCALE;
    char *bw_weight_param = NULL;

    // Parse params, extract BW_WEIGHT_SCALE if present
    // DO NOT use consensus_param_bw_weight_scale() in this code!
    // The consensus is not formed yet!
    /* XXXX Extract this code into a common function. Or not: #19011. */
    if (params) {
      if (strcmpstart(params, "bwweightscale=") == 0)
        bw_weight_param = params;
      else
        bw_weight_param = strstr(params, " bwweightscale=");
    }

    if (bw_weight_param) {
      int ok=0;
      char *eq = strchr(bw_weight_param, '=');
      if (eq) {
        weight_scale = tor_parse_long(eq+1, 10, 1, INT32_MAX, &ok,
                                         NULL);
        if (!ok) {
          log_warn(LD_DIR, "Bad element '%s' in bw weight param",
              escaped(bw_weight_param));
          weight_scale = BW_WEIGHT_SCALE;
        }
      } else {
        log_warn(LD_DIR, "Bad element '%s' in bw weight param",
            escaped(bw_weight_param));
        weight_scale = BW_WEIGHT_SCALE;
      }
    }

    added_weights = networkstatus_compute_bw_weights_v10(chunks, G, M, E, D,
                                                         T, weight_scale);
  }

  /* Add a signature. */
  {
    char digest[DIGEST256_LEN];
    char fingerprint[HEX_DIGEST_LEN+1];
    char signing_key_fingerprint[HEX_DIGEST_LEN+1];
    digest_algorithm_t digest_alg =
      flavor == FLAV_NS ? DIGEST_SHA1 : DIGEST_SHA256;
    size_t digest_len =
      flavor == FLAV_NS ? DIGEST_LEN : DIGEST256_LEN;
    const char *algname = crypto_digest_algorithm_get_name(digest_alg);
    char *signature;

    smartlist_add(chunks, tor_strdup("directory-signature "));

    /* Compute the hash of the chunks. */
    crypto_digest_smartlist(digest, digest_len, chunks, "", digest_alg);

    /* Get the fingerprints */
    crypto_pk_get_fingerprint(identity_key, fingerprint, 0);
    crypto_pk_get_fingerprint(signing_key, signing_key_fingerprint, 0);

    /* add the junk that will go at the end of the line. */
    if (flavor == FLAV_NS) {
      smartlist_add_asprintf(chunks, "%s %s\n", fingerprint,
                   signing_key_fingerprint);
    } else {
      smartlist_add_asprintf(chunks, "%s %s %s\n",
                   algname, fingerprint,
                   signing_key_fingerprint);
    }
    /* And the signature. */
    if (!(signature = router_get_dirobj_signature(digest, digest_len,
                                                  signing_key))) {
      log_warn(LD_BUG, "Couldn't sign consensus networkstatus.");
      goto done;
    }
    smartlist_add(chunks, signature);

    if (legacy_id_key_digest && legacy_signing_key) {
      smartlist_add(chunks, tor_strdup("directory-signature "));
      base16_encode(fingerprint, sizeof(fingerprint),
                    legacy_id_key_digest, DIGEST_LEN);
      crypto_pk_get_fingerprint(legacy_signing_key,
                                signing_key_fingerprint, 0);
      if (flavor == FLAV_NS) {
        smartlist_add_asprintf(chunks, "%s %s\n", fingerprint,
                     signing_key_fingerprint);
      } else {
        smartlist_add_asprintf(chunks, "%s %s %s\n",
                     algname, fingerprint,
                     signing_key_fingerprint);
      }

      if (!(signature = router_get_dirobj_signature(digest, digest_len,
                                                    legacy_signing_key))) {
        log_warn(LD_BUG, "Couldn't sign consensus networkstatus.");
        goto done;
      }
      smartlist_add(chunks, signature);
    }
  }

  result = smartlist_join_strings(chunks, "", 0, NULL);

  {
    networkstatus_t *c;
    if (!(c = networkstatus_parse_vote_from_string(result, NULL,
                                                   NS_TYPE_CONSENSUS))) {
      log_err(LD_BUG, "Generated a networkstatus consensus we couldn't "
              "parse.");
      tor_free(result);
      goto done;
    }
    // Verify balancing parameters
    if (added_weights) {
      networkstatus_verify_bw_weights(c, consensus_method);
    }
    networkstatus_vote_free(c);
  }

 done:

  dircollator_free(collator);
  tor_free(client_versions);
  tor_free(server_versions);
  tor_free(packages);
  SMARTLIST_FOREACH(flags, char *, cp, tor_free(cp));
  smartlist_free(flags);
  SMARTLIST_FOREACH(chunks, char *, cp, tor_free(cp));
  smartlist_free(chunks);
  SMARTLIST_FOREACH(param_list, char *, cp, tor_free(cp));
  smartlist_free(param_list);

  return result;
}