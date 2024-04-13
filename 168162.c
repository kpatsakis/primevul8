void receiveFromReference()
try
{
  string packet;
  ComboAddress remote;
  int res=waitForData(s_socket->getHandle(), g_timeoutMsec/1000, 1000*(g_timeoutMsec%1000));
  
  if(res < 0 || res==0)
    return;

  while(s_socket->recvFromAsync(packet, remote)) {
    try {
      s_weanswers++;
      MOADNSParser mdp(false, packet.c_str(), packet.length());
      if(!mdp.d_header.qr) {
        cout<<"Received a question from our reference nameserver!"<<endl;
        continue;
      }

      typedef qids_t::index<AssignedIDTag>::type qids_by_id_index_t;
      qids_by_id_index_t& idindex=qids.get<AssignedIDTag>();
      qids_by_id_index_t::const_iterator found=idindex.find(ntohs(mdp.d_header.id));
      if(found == idindex.end()) {
        if(!g_quiet)      
          cout<<"Received an answer ("<<mdp.d_qname<<") from reference nameserver with id "<<mdp.d_header.id<<" which we can't match to a question!"<<endl;
        s_weunmatched++;
        continue;
      }

      QuestionData qd=*found;    // we have to make a copy because we reinsert below      
      qd.d_newAnswers=mdp.d_answers;
      qd.d_newRcode=mdp.d_header.rcode;
      idindex.replace(found, qd);
      if(qd.d_origRcode!=-1) {
	qids_t::const_iterator iter= qids.project<0>(found);
	measureResultAndClean(iter);
      }
    }
    catch(MOADNSException &e)
    {
      s_wednserrors++;
    }
    catch(std::out_of_range &e)
    {
      s_wednserrors++;
    }
    catch(std::exception& e) 
    {
      s_wednserrors++;
    }
  }
}