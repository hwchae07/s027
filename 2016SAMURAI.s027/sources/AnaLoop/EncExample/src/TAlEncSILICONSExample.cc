#include "TAlEncSILICONSExample.hh"

#include <iostream>

#include "TClonesArray.h"
#include "TMath.h"

#include "TArtAnaLoop.hh"
#include "TArtAnaLoopManager.hh"
#include "TArtAnaFile.hh"
#include "TArtAnaClock.hh"

#include "TArtCalibSILICONS.hh"
#include "TArtSILICONS.hh"
#include "TArtSILICONSParameters.hh"

#include "EAnalyser.hh"
#include "EWNum.hh"

#include "TArtStoreManager.hh"

TAlEncSILICONSExample::TAlEncSILICONSExample()
  : TAlEncSub(), siliconsparameters(0), calibsilicons(0)
{;}

TAlEncSILICONSExample::~TAlEncSILICONSExample()
{
  Destruct();
}

void TAlEncSILICONSExample::Construct()
{
  siliconsparameters = TArtSILICONSParameters::Instance();
  siliconsparameters->LoadParameter((char*)"db/SILICONS.xml");
  //  siliconsparameters->PrintListOfPara();
  calibsilicons = new TArtCalibSILICONS; //must be called after parameter is constructed
  BetaValueForSILICONSAnalysis = atof(std::getenv("SAMURAI_SILICONS_BETA")); 
  
  TArtCore::Info(__FILE__,"\e[35m\e[1mBeta for Doppler correction in SILICONS: %f\e[0m",BetaValueForSILICONSAnalysis);
  TArtCore::Info(__FILE__,"\e[35m\e[1mexport SAMURAI_SILICONS_BETA=0.XX to change\e[0m");
  if(!fAnaLoop->UsingAnaFile()){
    hSi_adc = new TH2F("Si_adc","Si_adc",1000,0,4000,140,0,140);
    hSi_tdc = new TH2F("Si_tdc","Si_tdc",1000,0,4000,140,0,140);
  }else{
    /* nothing */;
  }
}

void TAlEncSILICONSExample::PreCalculate()
{
  calibsilicons->ClearData();
}

bool TAlEncSILICONSExample::Calculate()
{
  calibsilicons->ReconstructData();
  //  calibsilicons->DopplerCorrect(0.616);
  //calibsilicons->DopplerCorrect(0.602);
  //Done in the reconstructor;

  using namespace Analyser;
  using namespace WNum::SILICONS;

  TArtStoreManager * sman = TArtStoreManager::Instance();
  TClonesArray *Sis = (TClonesArray *)sman->FindDataContainer("SILICONS");

  int mult=0;
  for(int i=0;i<Sis->GetEntries();i++){
    TArtSILICONS *Si = (TArtSILICONS *)Sis->At(i);

    Int_t id = Si->GetID();
    if(!fAnaLoop->UsingAnaFile()){
      hSi_adc->Fill(Si->GetRawADC(), id);
      hSi_tdc->Fill(Si->GetRawTDC(), id);
    }else{
      fAnaLoop->Add(SILICONS, ID,            id, id);
      fAnaLoop->Add(SILICONS, Layer,         id, Si->GetLayer());

      fAnaLoop->Add(SILICONS, PosX,          id, Si->GetPosX());
      fAnaLoop->Add(SILICONS, PosY,          id, Si->GetPosY());
      fAnaLoop->Add(SILICONS, PosZ,          id, Si->GetPosZ());

      fAnaLoop->Add(SILICONS, RawADC,        id, Si->GetRawADC());
      fAnaLoop->Add(SILICONS, Energy,        id, Si->GetEnergy());
      
      fAnaLoop->Add(SILICONS, RawTDC,        id, Si->GetRawTDC());
      fAnaLoop->Add(SILICONS, Time,          id, Si->GetTime());
      fAnaLoop->Add(SILICONS, TimeOffseted,  id, Si->GetTimeOffseted());
           
      if(Si->GetTime()>0) ++mult;
    }      
  }
  fAnaLoop->Add(SILICONS, Multiplicity, 0, mult);

  return true;
}

void TAlEncSILICONSExample::Destruct()
{
  delete calibsilicons;
  calibsilicons = 0;
  if(siliconsparameters) siliconsparameters->Delete();
  siliconsparameters = 0;
}

const char* TAlEncSILICONSExample::ClassName() const
{
  return "TAlEncSILICONSExample";
}

TAlEncSILICONSExampleFactory::TAlEncSILICONSExampleFactory()
 : TAlEncFactory()
{;}

TAlEncSILICONSExampleFactory::~TAlEncSILICONSExampleFactory()
{;}

TAlEncSub* TAlEncSILICONSExampleFactory::CreateEnc()
{
  return new TAlEncSILICONSExample;
}

