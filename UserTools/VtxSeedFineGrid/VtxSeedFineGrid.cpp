#include "VtxSeedFineGrid.h"

VtxSeedFineGrid::VtxSeedFineGrid():Tool(){}


bool VtxSeedFineGrid::Initialise(std::string configfile, DataModel &data){

  /////////////////// Useful header ///////////////////////
  if(configfile!="") m_variables.Initialise(configfile); // loading config file
  //m_variables.Print();
  m_variables.Get("verbosity", verbosity);
  m_variables.Get("useTrueDir", useTrueDir);
  m_variables.Get("useMRDTrack", useMRDTrack);


  m_data= &data; //assigning transient data pointer
  /////////////////////////////////////////////////////////////////
  vSeedVtxList = new std::vector<RecoVertex>;

  return true;
}


bool VtxSeedFineGrid::Execute(){
	Log("VtxSeedFineGrid Tool: Executing", v_debug, verbosity);

	auto* annie_event = m_data->Stores["RecoEvent"];
	if (!annie_event) {
		Log("Error: VtxSeedFineGrid tool could not find the RecoEvent Store", v_error, verbosity);
		return false;
	}

	m_data->Stores.at("RecoEvent")->Get("vSeedVtxList", vSeedVtxList);

	auto get_vtx = m_data->Stores.at("RecoEvent")->Get("TrueVertex", fTrueVertex);  ///> Get digits from "RecoEvent" 
	if (!get_vtx) {
		Log("LikelihoodFitterCheck  Tool: Error retrieving TrueVertex! ", v_error, verbosity);
		return false;
	}

	// Retrive digits from RecoEvent
	auto get_digit = m_data->Stores.at("RecoEvent")->Get("RecoDigit", fDigitList);  ///> Get digits from "RecoEvent" 
	if (!get_digit) {
		Log("LikelihoodFitterCheck  Tool: Error retrieving RecoDigits,no digit from the RecoEvent!", v_error, verbosity);
		return false;
	}

	if (!(useTrueDir || useMRDTrack)) {
		Log("Using simple direction (not yet implemented)", v_debug, verbosity);
	}

	if (useTrueDir && useMRDTrack) {
		Log("Unable to use two directions; defaulting to true direction", v_debug, verbosity);	//will change to not use true, to be more general, once MRD track usage is tested and acceptable
		useMRDTrack = 0;
	}

	Center = this->FindCenter();
	this->GenerateFineGrid();
	std::cout << "Center: " << Center.X() << ", " << Center.Y() << ", " << Center.Z() << endl;
	m_data->Stores.at("RecoEvent")->Set("vSeedVtxList", vSeedVtxList, true);

  return true;
}


bool VtxSeedFineGrid::Finalise(){
	//delete vSeedVtxList; vSeedVtxList = 0;
	Log("VtxSeedFineGrid exitting", v_debug, verbosity);
  return true;
}

Position VtxSeedFineGrid::FindCenter() {
	double recoVtxX, recoVtxY, recoVtxZ, recoVtxT, recoDirX, recoDirY, recoDirZ;
	double trueVtxX, trueVtxY, trueVtxZ, trueVtxT, trueDirX, trueDirY, trueDirZ;
	double seedX, seedY, seedZ, seedT, seedDirX, seedDirY, seedDirZ;
	double peakX, peakY, peakZ, bestFOM;
	double ConeAngle = Parameters::CherenkovAngle();

	// Get true Vertex information
	Position vtxPos = fTrueVertex->GetPosition();
	Direction vtxDir = fTrueVertex->GetDirection();
	trueVtxX = vtxPos.X();
	trueVtxY = vtxPos.Y();
	trueVtxZ = vtxPos.Z();
	trueVtxT = fTrueVertex->GetTime();
	trueDirX = vtxDir.X();
	trueDirY = vtxDir.Y();
	trueDirZ = vtxDir.Z();
	peakX = trueVtxX;
	peakY = trueVtxY;
	peakZ = trueVtxZ;
	bestFOM = 0;
	RecoVertex iSeed;
	RecoVertex thisCenterSeed;

	if (verbosity > 0) cout << "True vertex  = (" << trueVtxX << ", " << trueVtxY << ", " << trueVtxZ << ", " << trueVtxT << ", " << trueDirX << ", " << trueDirY << ", " << trueDirZ << ")" << endl;

	FoMCalculator * myFoMCalculator = new FoMCalculator();
	VertexGeometry* myvtxgeo = VertexGeometry::Instance();
	myvtxgeo->LoadDigits(fDigitList);
	myFoMCalculator->LoadVertexGeometry(myvtxgeo); //Load vertex geometry

	// fom at true vertex position
	double fom = -999.999 * 100;
	double timefom = -999.999 * 100;
	double conefom = -999.999 * 100;
	myvtxgeo->CalcExtendedResiduals(trueVtxX, trueVtxY, trueVtxZ, 0.0, trueDirX, trueDirY, trueDirZ);
	myFoMCalculator->TimePropertiesLnL(trueVtxT, timefom);
	myFoMCalculator->ConePropertiesFoM(ConeAngle, conefom);
	fom = timefom * 0.5 + conefom * 0.5;
	if (verbosity > 0)  cout << "VtxSeedFineGrid Tool: " << "FOM at true vertex = " << fom << endl;

	for (int m = 0; m < vSeedVtxList->size(); m++) {
		iSeed = vSeedVtxList->at(m);
		seedX = iSeed.GetPosition().X();
		seedY = iSeed.GetPosition().Y();
		seedZ = iSeed.GetPosition().Z();
		seedT = trueVtxT;
		if (useTrueDir) {
			seedDirX = trueDirX;
			seedDirY = trueDirY;
			seedDirZ = trueDirZ;
		}
		else if (useMRDTrack) {
			iSeed.SetDirection(this->findDirectionMRD());
			seedDirX = iSeed.GetDirection().X();
			seedDirY = iSeed.GetDirection().Y();
			seedDirZ = iSeed.GetDirection().Z();
		}
		myvtxgeo->CalcExtendedResiduals(seedX, seedY, seedZ, seedT, seedDirX, seedDirY, seedDirZ);
		int nhits = myvtxgeo->GetNDigits();
		double meantime = myFoMCalculator->FindSimpleTimeProperties(ConeAngle);
		Double_t fom = -999.999 * 100;
		double timefom = -999.999 * 100;
		double conefom = -999.999 * 100;
		double coneAngle = 42.0;
		myFoMCalculator->TimePropertiesLnL(meantime, timefom);
		myFoMCalculator->ConePropertiesFoM(coneAngle, conefom);
		fom = timefom * 0.5 + conefom * 0.5;
	
		if (fom > bestFOM) {
			bestFOM = fom;
			peakX = seedX;
			peakY = seedY;
			peakZ = seedZ;
			thisCenterSeed = iSeed;
		}

	//	return thisCenterSeed.GetPosition();

	}
	return thisCenterSeed.GetPosition();
}

void VtxSeedFineGrid::GenerateFineGrid() {
	Position Seed;
	RecoVertex thisFineSeed;
	vSeedVtxList->clear();
	double medianTime;
	//double length = NSeeds something.  TODO for now setting to standard size 25x25x25, with seeds 5cm apart.
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 5; k++) {
				Seed.SetZ(Center.Z() - 10 + 5 * i);
				Seed.SetX(Center.X() - 10 + 5 * j);
				Seed.SetY(Center.Y() - 10 + 5 * k);
				//medianTime = this->GetMedianSeedTime(Seed);
				thisFineSeed.SetVertex(Seed, medianTime);
				vSeedVtxList->push_back(thisFineSeed);

			}
		}
	}

}

Direction VtxSeedFineGrid::findDirectionMRD() {
	std::vector<BoostStore>* Tracks;
	m_data->Stores["MRDTracks"]->Get("MRDTracks", Tracks);
	m_data->Stores["MRDTracks"]->Get("NumMrdTracks", numtracksinev);

	if (numtracksinev > 1) Log("Multiple tracks need work; just using first for now", v_debug, verbosity);
	double dirX, dirY, dirZ;
	Direction startVertex, endVertex, result;
	BoostStore* thisTrack = &(Tracks->at(0));
	
	thisTrack->Get("StartVertex", startVertex);
	thisTrack->Get("StopVertex", endVertex);
	dirX = endVertex.X() - startVertex.X();
	dirY = endVertex.Y() - startVertex.Y();
	dirZ = endVertex.Z() - startVertex.Y();
	result.SetX(dirX);
	result.SetY(dirY);
	result.SetZ(dirZ);

	return result;

}

/*double VtxSeedFineGrid::GetMedianSeedTime(Position pos) {
	double digitx, digity, digitz, digittime;
	double dx, dy, dz, dr;
	double fC, fN;
	double seedtime;
	int fThisDigit;
	std::vector<double> extraptimes;
	for (int entry = 0; entry < vSeedDigitList.size(); entry++) {
		fThisDigit = vSeedDigitList.at(entry);
		digitx = fDigitList->at(fThisDigit).GetPosition().X();
		digity = fDigitList->at(fThisDigit).GetPosition().Y();
		digitz = fDigitList->at(fThisDigit).GetPosition().Z();
		digittime = fDigitList->at(fThisDigit).GetCalTime();
		//Now, find distance to seed position
		dx = digitx - pos.X();
		dy = digity - pos.Y();
		dz = digitz - pos.Z();
		dr = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));

		//Back calculate to the vertex time using speed of light in H20
		//Very rough estimate; ignores muon path before Cherenkov production
		//TODO: add charge weighting?  Kinda like CalcSimpleVertex?
		fC = Parameters::SpeedOfLight();
		fN = Parameters::Index0();
		seedtime = digittime - (dr / (fC / fN));
		extraptimes.push_back(seedtime);
	}
	//return the median of the extrapolated vertex times
	size_t median_index = extraptimes.size() / 2;
	std::nth_element(extraptimes.begin(), extraptimes.begin() + median_index, extraptimes.end());
	return extraptimes[median_index];
}*/
