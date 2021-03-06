#include "PhaseIITreeMakerDoE.h"

PhaseIITreeMakerDoE::PhaseIITreeMakerDoE():Tool(){}


bool PhaseIITreeMakerDoE::Initialise(std::string configfile, DataModel &data){

  /////////////////// Usefull header ///////////////////////
  if(configfile!="")  m_variables.Initialise(configfile); //loading config file
  //m_variables.Print();

  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////
  
  m_variables.Get("verbose", verbosity);
  m_variables.Get("muonMCTruth_fill", muonMCTruth_fill);
  m_variables.Get("muonRecoDebug_fill", muonRecoDebug_fill);
  m_variables.Get("muonTruthRecoDiff_fill", muonTruthRecoDiff_fill);
  m_variables.Get("pionKaonCount_fill", pionKaonCount_fill);

  std::string output_filename;
  m_variables.Get("OutputFile", output_filename);
  fOutput_tfile = new TFile(output_filename.c_str(), "recreate");
  fRecoTree = new TTree("phaseII", "ANNIE Phase II Reconstruction Tree");
  //Metadata for Events
  fRecoTree->Branch("McEntryNumber",&fMCEventNum,"McEntryNumber/I");
  fRecoTree->Branch("triggerNumber",&fMCTriggerNum,"triggerNumber/I");
  fRecoTree->Branch("eventNumber",&fEventNumber,"eventNumber/I");

  //Hit information (PMT and LAPPD)
  //Always output in Phase II Reco Tree
  fRecoTree->Branch("nhits",&fNhits,"fNhits/I");
  fRecoTree->Branch("filter",&fIsFiltered);
  fRecoTree->Branch("digitX",&fDigitX);
  fRecoTree->Branch("digitY",&fDigitY);
  fRecoTree->Branch("digitZ",&fDigitZ);
  fRecoTree->Branch("digitT",&fDigitT);
  fRecoTree->Branch("digitQ",&fDigitQ);
  fRecoTree->Branch("digitType", &fDigitType);
  fRecoTree->Branch("digitDetID", &fDigitDetID);

  //Reconstructed variables after full Muon Reco Analysis
  //Always output in Phase II Reco Tree
  fRecoTree->Branch("recoVtxX",&fRecoVtxX,"recoVtxX/D");
  fRecoTree->Branch("recoVtxY",&fRecoVtxY,"recoVtxY/D");
  fRecoTree->Branch("recoVtxZ",&fRecoVtxZ,"recoVtxZ/D");
  fRecoTree->Branch("recoVtxTime",&fRecoVtxTime,"recoVtxTime/D");
  fRecoTree->Branch("recoDirX",&fRecoDirX,"recoDirX/D");
  fRecoTree->Branch("recoDirY",&fRecoDirY,"recoDirY/D");
  fRecoTree->Branch("recoDirZ",&fRecoDirZ,"recoDirZ/D");
  fRecoTree->Branch("recoTheta",&fRecoTheta,"recoTheta/D");
  fRecoTree->Branch("recoPhi",&fRecoPhi,"recoPhi/D");
  fRecoTree->Branch("recoVtxFOM",&fRecoVtxFOM,"recoVtxFOM/D");
  fRecoTree->Branch("recoStatus",&fRecoStatus,"recoStatus/I");
  
  //MC truth information for muons
  //Output to tree when muonMCTruth_fill = 1 in config
  if (muonMCTruth_fill){
    fRecoTree->Branch("trueVtxX",&fTrueVtxX,"trueVtxX/D");
    fRecoTree->Branch("trueVtxY",&fTrueVtxY,"trueVtxY/D");
    fRecoTree->Branch("trueVtxZ",&fTrueVtxZ,"trueVtxZ/D");
    fRecoTree->Branch("trueVtxTime",&fTrueVtxTime,"trueVtxTime/D");
    fRecoTree->Branch("trueDirX",&fTrueDirX,"trueDirX/D");
    fRecoTree->Branch("trueDirY",&fTrueDirY,"trueDirY/D");
    fRecoTree->Branch("trueDirZ",&fTrueDirZ,"trueDirZ/D");
    fRecoTree->Branch("trueTheta",&fTrueTheta,"trueTheta/D");
    fRecoTree->Branch("truePhi",&fTruePhi,"truePhi/D");
  }
  
  // Reconstructed variables from each step in Muon Reco Analysis
  // Currently output when muonRecoDebug_fill = 1 in config 
  if (muonRecoDebug_fill){
    fRecoTree->Branch("seedVtxX",&fSeedVtxX); 
    fRecoTree->Branch("seedVtxY",&fSeedVtxY); 
    fRecoTree->Branch("seedVtxZ",&fSeedVtxZ);
    fRecoTree->Branch("seedVtxFOM",&fSeedVtxFOM); 
    fRecoTree->Branch("seedVtxTime",&fSeedVtxTime,"seedVtxTime/D");
    
    fRecoTree->Branch("pointPosX",&fPointPosX,"pointPosX/D");
    fRecoTree->Branch("pointPosY",&fPointPosY,"pointPosY/D");
    fRecoTree->Branch("pointPosZ",&fPointPosZ,"pointPosZ/D");
    fRecoTree->Branch("pointPosTime",&fPointPosTime,"pointPosTime/D");
    fRecoTree->Branch("pointPosFOM",&fPointPosFOM,"pointPosFOM/D");
    fRecoTree->Branch("pointPosStatus",&fPointPosStatus,"pointPosStatus/I");
    
    fRecoTree->Branch("pointDirX",&fPointDirX,"pointDirX/D");
    fRecoTree->Branch("pointDirY",&fPointDirY,"pointDirY/D");
    fRecoTree->Branch("pointDirZ",&fPointDirZ,"pointDirZ/D");
    fRecoTree->Branch("pointDirTime",&fPointDirTime,"pointDirTime/D");
    fRecoTree->Branch("pointDirStatus",&fPointDirStatus,"pointDirStatus/I");
    fRecoTree->Branch("pointDirFOM",&fPointDirFOM,"pointDirFOM/D");
    
    fRecoTree->Branch("pointVtxPosX",&fPointVtxPosX,"pointVtxPosX/D");
    fRecoTree->Branch("pointVtxPosY",&fPointVtxPosY,"pointVtxPosY/D");
    fRecoTree->Branch("pointVtxPosZ",&fPointVtxPosZ,"pointVtxPosZ/D");
    fRecoTree->Branch("pointVtxTime",&fPointVtxTime,"pointVtxTime/D");
    fRecoTree->Branch("pointVtxDirX",&fPointVtxDirX,"pointVtxDirX/D");
    fRecoTree->Branch("pointVtxDirY",&fPointVtxDirY,"pointVtxDirY/D");
    fRecoTree->Branch("pointVtxDirZ",&fPointVtxDirZ,"pointVtxDirZ/D");
    fRecoTree->Branch("pointVtxFOM",&fPointVtxFOM,"pointVtxFOM/D");
    fRecoTree->Branch("pointVtxStatus",&fPointVtxStatus,"pointVtxStatus/I");
  } 

  // Difference in MC Truth and Muon Reconstruction Analysis
  // Output to tree when muonTruthRecoDiff_fill = 1 in config
  if (muonTruthRecoDiff_fill){
    fRecoTree->Branch("DeltaVtxX",&fDeltaVtxX,"DeltaVtxX/D");
    fRecoTree->Branch("DeltaVtxY",&fDeltaVtxY,"DeltaVtxY/D");
    fRecoTree->Branch("DeltaVtxZ",&fDeltaVtxZ,"DeltaVtxZ/D");
    fRecoTree->Branch("DeltaVtxR",&fDeltaVtxR,"DeltaVtxR/D");
    fRecoTree->Branch("DeltaVtxT",&fDeltaVtxT,"DeltaVtxT/D");
    fRecoTree->Branch("DeltaParallel",&fDeltaParallel,"DeltaParallel/D");
    fRecoTree->Branch("DeltaPerpendicular",&fDeltaPerpendicular,"DeltaPerpendicular/D");
    fRecoTree->Branch("DeltaAzimuth",&fDeltaAzimuth,"DeltaAzimuth/D");
    fRecoTree->Branch("DeltaZenith",&fDeltaZenith,"DeltaZenith/D");
    fRecoTree->Branch("DeltaAngle",&fDeltaAngle,"DeltaAngle/D");
  } 

  // Pion and kaon counts as found in MC Truth based on PDG codes
  if (pionKaonCount_fill){
    fRecoTree->Branch("Pi0Count",&fPi0Count,"Pi0Count/I");
    fRecoTree->Branch("PiPlusCount",&fPiPlusCount,"PiPlusCount/I");
    fRecoTree->Branch("PiMinusCount",&fPiMinusCount,"PiMinusCount/I");
    fRecoTree->Branch("K0Count",&fK0Count,"K0Count/I");
    fRecoTree->Branch("KPlusCount",&fKPlusCount,"KPlusCount/I");
    fRecoTree->Branch("KMinusCount",&fKMinusCount,"KMinusCount/I");
  }

  return true;
}

bool PhaseIITreeMakerDoE::Execute(){
	Log("===========================================================================================",v_debug,verbosity);
  Log("PhaseIITreeMakerDoE Tool: Executing",v_debug,verbosity);

  // Reset variables
  this->ResetVariables();
  // Get a pointer to the ANNIEEvent Store
  auto* annie_event = m_data->Stores["RecoEvent"];
  if (!annie_event) {
    Log("Error: The PhaseITreeMaker tool could not find the RecoEvent Store", v_error, verbosity);
    return false;
  }

  // check if event passes the cut
  bool EventCutstatus = false;
  auto get_evtstatus = m_data->Stores.at("RecoEvent")->Get("EventCutStatus",EventCutstatus);
  if(!get_evtstatus) {
    Log("Error: The PhaseITreeMaker tool could not find the Event selection status", v_error, verbosity);
    return false;	
  }

  if(!EventCutstatus) {
  	Log("Message: This event doesn't pass the event selection. ", v_message, verbosity);
    return true;	
  }
  
  // MC entry number
  m_data->Stores.at("ANNIEEvent")->Get("MCEventNum",fMCEventNum);  
  
  // MC trigger number
  m_data->Stores.at("ANNIEEvent")->Get("MCTriggernum",fMCTriggerNum); 
  
  // ANNIE Event number
  m_data->Stores.at("ANNIEEvent")->Get("EventNumber",fEventNumber);
  
  std::string logmessage = "  Retriving information for MCEntry "+to_string(fMCEventNum)+
  	", MCTrigger "+ to_string(fMCTriggerNum) + ", EventNumber " + to_string(fEventNumber);
  Log(logmessage, v_message, verbosity);
  
  // Read digits
  std::vector<RecoDigit>* digitList = nullptr;
  auto get_digits = m_data->Stores.at("RecoEvent")->Get("RecoDigit",digitList);  ///> Get digits from "RecoEvent" 
  if(!get_digits) {
    Log("PhaseITreeMaker tool: no digit list in store!", v_error, verbosity);
    return false;	
  }
  fNhits = digitList->size();
  for( auto& digit : *digitList ){
    fDigitX.push_back(digit.GetPosition().X());
    fDigitY.push_back(digit.GetPosition().Y());
    fDigitZ.push_back(digit.GetPosition().Z());
    fDigitT.push_back(digit.GetCalTime());      
    fDigitQ.push_back(digit.GetCalCharge());
    fDigitType.push_back(digit.GetDigitType());
    fDigitDetID.push_back(digit.GetDetectorID());
  }
  
  // Read reconstructed Vertex
  RecoVertex* recovtx = 0;
  auto get_extendedvtx = m_data->Stores.at("RecoEvent")->Get("ExtendedVertex",recovtx); 
  if(!get_extendedvtx) {
    Log("Warning: The PhaseITreeMaker tool could not find ExtendedVertex. Continuing to build tree", v_message, verbosity);
  } else {
    fRecoVtxX = recovtx->GetPosition().X();
    fRecoVtxY = recovtx->GetPosition().Y();
    fRecoVtxZ = recovtx->GetPosition().Z();
    fRecoVtxTime = recovtx->GetTime();
    fRecoVtxFOM = recovtx->GetFOM();
    fRecoDirX = recovtx->GetDirection().X();
    fRecoDirY = recovtx->GetDirection().Y();
    fRecoDirZ = recovtx->GetDirection().Z();
    fRecoTheta = TMath::ACos(fRecoDirZ);
    if (fRecoDirX>0.0){
      fRecoPhi = atan(fRecoDirY/fRecoDirX);
    }
    if (fRecoDirX<0.0){
      fRecoPhi = atan(fRecoDirY/fRecoDirX);
      if( fRecoDirY>0.0) fRecoPhi += TMath::Pi();
      if( fRecoDirY<=0.0) fRecoPhi -= TMath::Pi();
    }
    if (fRecoDirX==0.0){
      if( fRecoDirY>0.0) fRecoPhi = 0.5*TMath::Pi();
      else if( fRecoDirY<0.0) fRecoPhi = -0.5*TMath::Pi();
      else fRecoPhi = 0;
    }
    fRecoStatus = recovtx->GetStatus();
  } 
  // Read True Vertex if flag is set   
  RecoVertex* truevtx = 0;
  auto get_muonMC = m_data->Stores.at("RecoEvent")->Get("TrueVertex",truevtx); 
  if(get_muonMC){ 
    fTrueVtxX = truevtx->GetPosition().X();
    fTrueVtxY = truevtx->GetPosition().Y();
    fTrueVtxZ = truevtx->GetPosition().Z();
    fTrueVtxTime = truevtx->GetTime();
    fTrueDirX = truevtx->GetDirection().X();
    fTrueDirY = truevtx->GetDirection().Y();
    fTrueDirZ = truevtx->GetDirection().Z();
    fTrueTheta = TMath::ACos(fTrueDirZ);
    if (fTrueDirX>0.0){
      fTruePhi = atan(fTrueDirY/fTrueDirX);
    }
    if (fTrueDirX<0.0){
      fTruePhi = atan(fTrueDirY/fTrueDirX);
      if( fTrueDirY>0.0) fTruePhi += TMath::Pi();
      if( fTrueDirY<=0.0) fTruePhi -= TMath::Pi();
    }
    if (fTrueDirX==0.0){
      if( fTrueDirY>0.0) fTruePhi = 0.5*TMath::Pi();
      else if( fTrueDirY<0.0) fTruePhi = -0.5*TMath::Pi();
      else fTruePhi = 0;
    }
  } else {
    Log("PhaseIITreeMakerDoE Tool: No MC Truth data found; is this MC?  Continuing to build remaining tree",v_message,verbosity);
  } 

  if (muonRecoDebug_fill){
    // Read Seed candidates   
    std::vector<RecoVertex>* seedvtxlist = 0;
    auto get_seedvtxlist = m_data->Stores.at("RecoEvent")->Get("vSeedVtxList",seedvtxlist);  ///> Get List of seeds from "RecoEvent"
    if(get_seedvtxlist){
      for( auto& seed : *seedvtxlist ){
        fSeedVtxX.push_back(seed.GetPosition().X());
        fSeedVtxY.push_back(seed.GetPosition().Y());
        fSeedVtxZ.push_back(seed.GetPosition().Z());
        fSeedVtxTime = seed.GetTime();
      }
    } else {  
  	Log("PhaseIITreeMakerDoE  Tool: No Seed List found.  Continuing to build tree ",v_message,verbosity); 
    }
    std::vector<double>* seedfomlist = 0;
    auto get_seedfomlist = m_data->Stores.at("RecoEvent")->Get("vSeedFOMList",seedfomlist);  ///> Get List of seed FOMs from "RecoEvent" 
    if(get_seedfomlist){
      for( auto& seedFOM : *seedfomlist ){
        fSeedVtxFOM.push_back(seedFOM);
      }
    } else {  
  	Log("PhaseIITreeMakerDoE  Tool: No Seed FOM List found.  Continuing to build tree ",v_message,verbosity); 
    }
    
    // Read PointPosition-fitted Vertex   
    RecoVertex* pointposvtx = 0;
    auto get_pointposdata = m_data->Stores.at("RecoEvent")->Get("PointPosition",pointposvtx);
    if(get_pointposdata){ 
      fPointPosX = pointposvtx->GetPosition().X();
      fPointPosY = pointposvtx->GetPosition().Y();
      fPointPosZ = pointposvtx->GetPosition().Z();
      fPointPosTime = pointposvtx->GetTime();
      fPointPosFOM = pointposvtx->GetFOM();
      fPointPosStatus = pointposvtx->GetStatus();
    } else{
      Log("PhaseIITreeMakerDoE Tool: No PointPosition Tool data found.  Continuing to build remaining tree",v_message,verbosity);
    }

    // Read PointDirection-fitted Vertex
    RecoVertex* pointdirvtx = 0;
    auto get_pointdirdata = m_data->Stores.at("RecoEvent")->Get("PointDirection",pointdirvtx);
    if(get_pointdirdata){ 
      fPointDirX = pointdirvtx->GetDirection().X();
      fPointDirY = pointdirvtx->GetDirection().Y();
      fPointDirZ = pointdirvtx->GetDirection().Z();
      fPointDirTime = pointdirvtx->GetTime();
      fPointDirFOM = pointdirvtx->GetFOM();
      fPointDirStatus = pointdirvtx->GetStatus();
    } else{
      Log("PhaseIITreeMakerDoE Tool: No PointDirection Tool data found.  Continuing to build remaining tree",v_message,verbosity);
    }
    
    // Read PointVertex Tool's fitted Vertex
    RecoVertex* pointvtx = 0;
    auto get_pointvtxdata = m_data->Stores.at("RecoEvent")->Get("PointVertex",pointvtx);
    if(get_pointvtxdata){ 
      fPointVtxPosX = pointvtx->GetPosition().X();
      fPointVtxPosY = pointvtx->GetPosition().Y();
      fPointVtxPosZ = pointvtx->GetPosition().Z();
      fPointVtxDirX = pointvtx->GetDirection().X();
      fPointVtxDirY = pointvtx->GetDirection().Y();
      fPointVtxDirZ = pointvtx->GetDirection().Z();
      fPointVtxTime = pointvtx->GetTime();
      fPointVtxFOM = pointvtx->GetFOM();
      fPointVtxStatus = pointvtx->GetStatus();
    } else{
      Log("PhaseIITreeMakerDoE Tool: No PointVertex Tool data found.  Continuing to build remaining tree",v_message,verbosity);
    }
  }

  if (pionKaonCount_fill){
    int pi0count, pipcount, pimcount, K0count, Kpcount, Kmcount;
    auto get_pi0 = m_data->Stores.at("RecoEvent")->Get("MCPi0Count",pi0count);
    auto get_pim = m_data->Stores.at("RecoEvent")->Get("MCPiMinusCount",pimcount);
    auto get_pip = m_data->Stores.at("RecoEvent")->Get("MCPiPlusCount",pipcount);
    auto get_k0 = m_data->Stores.at("RecoEvent")->Get("MCK0Count",K0count);
    auto get_km = m_data->Stores.at("RecoEvent")->Get("MCKMinusCount",Kmcount);
    auto get_kp = m_data->Stores.at("RecoEvent")->Get("MCKPlusCount",Kpcount);
    if(get_pi0 && get_pim && get_pip && get_k0 && get_km && get_kp) {
      // set values in tree to thouse grabbed from the RecoEvent Store
      fPi0Count = pi0count;
      fPiPlusCount = pipcount;
      fPiMinusCount = pimcount;
      fK0Count = K0count;
      fKPlusCount = Kpcount;
      fKMinusCount = Kmcount;
    } else {
      Log("PhaseIITreeMakerDoE Tool: Missing MC Pion/Kaon count information. Continuing to build remaining tree",v_message,verbosity);
    }
  } 

  if (muonTruthRecoDiff_fill){
    if (!get_muonMC || !get_extendedvtx) {
      Log("PhaseIITreeMakerDoE Tool: No True Muon Vertex or Extended Vertex information found.  Continuing to build remaining tree",v_message,verbosity);
    } else {
      //Let's fill in stuff from the RecoSummary
      fDeltaVtxX = fRecoVtxX - fTrueVtxX;
      fDeltaVtxY = fRecoVtxY - fTrueVtxY;
      fDeltaVtxZ = fRecoVtxZ - fTrueVtxZ;
      fDeltaVtxT = fRecoVtxTime - fTrueVtxTime;
      fDeltaVtxR = sqrt(pow(fDeltaVtxX,2) + pow(fDeltaVtxY,2) + pow(fDeltaVtxZ,2)); 
      fDeltaParallel = fDeltaVtxX*fRecoDirX + fDeltaVtxY*fRecoDirY + fDeltaVtxZ*fRecoDirZ;
      fDeltaPerpendicular = sqrt(pow(fDeltaVtxR,2) - pow(fDeltaParallel,2));
      fDeltaAzimuth = (fRecoTheta - fTrueTheta)/(TMath::Pi()/180.0);
      fDeltaZenith = (fRecoPhi - fTruePhi)/(TMath::Pi()/180.0); 
      double cosphi = fTrueDirX*fRecoDirX+fTrueDirY*fRecoDirY+fTrueDirZ*fRecoDirZ;
      double phi = TMath::ACos(cosphi); // radians
      double TheAngle = phi/(TMath::Pi()/180.0); // radians->degrees
      fDeltaAngle = TheAngle;
    }
  }

  fRecoTree->Fill();
  this->RecoSummary();
  return true;
}

bool PhaseIITreeMakerDoE::Finalise(){
	fOutput_tfile->cd();
	fRecoTree->Write();
	fOutput_tfile->Close();
	
	if(verbosity>0) cout<<"PhaseIITreeMakerDoE exitting"<<endl;

  return true;
}

void PhaseIITreeMakerDoE::ResetVariables() {
  // tree variables
  fMCEventNum = 0;
  fMCTriggerNum = 0;
  fEventNumber = 0;
  fNhits = 0;
  
  fTrueVtxX = 0;
  fTrueVtxY = 0;
  fTrueVtxZ = 0;
  fTrueVtxTime = 0;
  fTrueDirX = 0;
  fTrueDirY = 0;
  fTrueDirZ = 0;
  fTrueTheta = 0;
  fTruePhi = 0;
 
  if (muonRecoDebug_fill){ 
    fSeedVtxX.clear();
    fSeedVtxY.clear();
    fSeedVtxZ.clear();
    fSeedVtxFOM.clear();
    fSeedVtxTime = 0;
    fPointPosX = 0;
    fPointPosY = 0;
    fPointPosZ = 0;
    fPointPosTime = 0;
    fPointPosFOM = 0;
    fPointPosStatus = 0;
    fPointDirX = 0;
    fPointDirY = 0;
    fPointDirZ = 0;
    fPointDirTime = 0;
    fPointDirFOM = 0;
    fPointDirStatus = 0;
    fPointVtxPosX = 0;
    fPointVtxPosY = 0;
    fPointVtxPosZ = 0;
    fPointVtxDirX = 0;
    fPointVtxDirY = 0;
    fPointVtxDirZ = 0;
    fPointVtxTime = 0;
    fPointVtxStatus = 0;
    fPointVtxFOM = 0;
  }

  fRecoVtxX = 0;
  fRecoVtxY = 0;
  fRecoVtxZ = 0;
  fRecoStatus = 0;
  fRecoVtxTime = 0;
  fRecoVtxFOM = 0;
  fRecoDirX = 0;
  fRecoDirY = 0;
  fRecoDirZ = 0;
  fRecoTheta = 0;
  fRecoPhi = 0;
  fIsFiltered.clear();
  fDigitX.clear();
  fDigitY.clear();
  fDigitZ.clear();
  fDigitT.clear();
  fDigitQ.clear();
  fDigitType.clear();
  fDigitDetID.clear();	
  
  if (muonTruthRecoDiff_fill){ 
    fDeltaVtxX = 0;
    fDeltaVtxY = 0;
    fDeltaVtxZ = 0;
    fDeltaVtxR = 0;
    fDeltaVtxT = 0;
    fDeltaParallel = 0;
    fDeltaPerpendicular = 0;
    fDeltaAzimuth = 0;
    fDeltaZenith = 0;
    fDeltaAngle = 0;
  }

  if (pionKaonCount_fill){
    fPi0Count = -1;
    fPiPlusCount = -1;
    fPiMinusCount = -1;
    fK0Count = -1;
    fKPlusCount = -1;
    fKMinusCount = -1;
  }
}

void PhaseIITreeMakerDoE::RecoSummary() {

  // get reconstruction output
  double dx = fRecoVtxX - fTrueVtxX;
  double dy = fRecoVtxY - fTrueVtxY;
  double dz = fRecoVtxZ - fTrueVtxZ;
  double dt = fRecoVtxTime - fTrueVtxTime;
  double deltaR = sqrt(dx*dx + dy*dy + dz*dz);
  double cosphi = 0., phi = 0., DeltaAngle = 0.;
  cosphi = fTrueDirX*fRecoDirX+fTrueDirY*fRecoDirY+fTrueDirZ*fRecoDirZ;
  phi = TMath::ACos(cosphi); // radians
  DeltaAngle = phi/(TMath::Pi()/180.0); // radians->degrees
  std::cout << "============================================================================"<<std::endl;
  std::cout << " Event number " << fEventNumber << std::endl;
  std::cout << "  trueVtx=(" << fTrueVtxX << ", " << fTrueVtxY << ", " << fTrueVtxZ <<", "<< fTrueVtxTime<< std::endl
            << "           " << fTrueDirX << ", " << fTrueDirY << ", " << fTrueDirZ << ") " << std::endl;
  std::cout << "  recoVtx=(" << fRecoVtxX << ", " << fRecoVtxY << ", " << fRecoVtxZ <<", "<< fRecoVtxTime << std::endl
            << "           " << fRecoDirX << ", " << fRecoDirY << ", " << fRecoDirZ << ") " << std::endl;
  std::cout << "  DeltaR = "<<deltaR<<"[cm]"<<"\t"<<"  DeltaAngle = "<<DeltaAngle<<" [degree]"<<std::endl;
  std::cout << "  FOM = " << fRecoVtxFOM << std::endl;
  std::cout << "  RecoStatus = " << fRecoStatus <<std::endl;
  std::cout << std::endl;
}
