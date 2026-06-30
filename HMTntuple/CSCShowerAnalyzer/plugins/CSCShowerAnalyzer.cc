#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "L1Trigger/CSCTriggerPrimitives/interface/CSCMotherboard.h"

#include "DataFormats/MuonDetId/interface/CSCTriggerNumbering.h"
#include "DataFormats/CSCDigi/interface/CSCShowerDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCALCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCCorrelatedLCTDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCComparatorDigiCollection.h"
#include "DataFormats/CSCDigi/interface/CSCWireDigiCollection.h"
#include "TH2.h"
#include <string>
#include <iostream>


class CSCShowerAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources> {
public:
  explicit CSCShowerAnalyzer(const edm::ParameterSet& ps);
  ~CSCShowerAnalyzer() override;

  void analyze(const edm::Event&, const edm::EventSetup&) override;

  //void endJob() override;
  void beginJob() override;
private:
  bool areSameShowers(const CSCShowerDigi& lhs, const CSCShowerDigi& rhs) const;
  bool areSameShowers(const CSCShowerDigi& lhs, const CSCShowerDigi& rhs,bool isALCT) const;
  void fillhist(TH2D * h , int chamber, int sr) const;

  bool debug_;
  edm::InputTag compDigiProducer_;
  edm::InputTag wireDigiProducer_;
  edm::EDGetTokenT<CSCALCTDigiCollection> dataALCT_token_;
  edm::EDGetTokenT<CSCCorrelatedLCTDigiCollection> dataLCT_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> dataALCTShower_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> dataALCTShower_TMB_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> emulALCTShower_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> dataCLCTShower_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> emulCLCTShower_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> dataLCTShower_token_;
  edm::EDGetTokenT<CSCShowerDigiCollection> emulLCTShower_token_;
  edm::EDGetTokenT<CSCComparatorDigiCollection> comp_token_;
  edm::EDGetTokenT<CSCWireDigiCollection> wire_token_;

  TH1D* counters_;
  TH1D* h_clctBx_data_nom_  ;
  TH1D* h_clctBx_data_tight_;
  TH1D* h_clctBx_emul_nom_  ;
  TH1D* h_clctBx_emul_tight_;
  TH1D* h_alctBx_data_nom_  ;
  TH1D* h_alctBx_data_tight_;
  TH1D* h_alctBx_emul_nom_  ;
  TH1D* h_alctBx_emul_tight_;
  TH1D* h_lctBx_data_nom_  ;
  TH1D* h_lctBx_data_tight_;
  TH1D* h_lctBx_emul_nom_  ;
  TH1D* h_lctBx_emul_tight_;


  TH2D* h_alct_Bx_data_v_emul;
  TH2D* h_clct_Bx_data_v_emul;
  TH2D* h_lct_Bx_data_v_emul;

  TH2D* h_alct_nom_Bx_v_station;
  TH2D* h_clct_nom_Bx_v_station;
  TH2D* h_lct_nom_Bx_v_station;
  TH2D* h_alct_tight_Bx_v_station;
  TH2D* h_clct_tight_Bx_v_station;
  TH2D* h_lct_tight_Bx_v_station;

  TH2D* h_alct_thres_data_v_emul;
  TH2D* h_clct_thres_data_v_emul;
  TH2D* h_lct_thres_data_v_emul;

  TH2D* lctShowerDataNomSummary_denom_;
  TH2D* lctShowerDataNomSummary_num_;
  TH2D* alctShowerDataNomSummary_denom_;
  TH2D* alctShowerDataNomSummary_num_;
  TH2D* clctShowerDataNomSummary_denom_;
  TH2D* clctShowerDataNomSummary_num_;

  TH2D* lctShowerEmulNomSummary_denom_;
  TH2D* lctShowerEmulNomSummary_num_;
  TH2D* alctShowerEmulNomSummary_denom_;
  TH2D* alctShowerEmulNomSummary_num_;
  TH2D* clctShowerEmulNomSummary_denom_;
  TH2D* clctShowerEmulNomSummary_num_;

  TH2D* lctShowerDataTightSummary_denom_;
  TH2D* lctShowerDataTightSummary_num_;
  TH2D* alctShowerDataTightSummary_denom_;
  TH2D* alctShowerDataTightSummary_num_;
  TH2D* clctShowerDataTightSummary_denom_;
  TH2D* clctShowerDataTightSummary_num_;

  TH2D* lctShowerEmulTightSummary_denom_;
  TH2D* lctShowerEmulTightSummary_num_;
  TH2D* alctShowerEmulTightSummary_denom_;
  TH2D* alctShowerEmulTightSummary_num_;
  TH2D* clctShowerEmulTightSummary_denom_;
  TH2D* clctShowerEmulTightSummary_num_;
};


CSCShowerAnalyzer::CSCShowerAnalyzer(const edm::ParameterSet& ps)
    : dataALCT_token_(consumes(ps.getParameter<edm::InputTag>("dataALCT"))), // check ALCT in data
      dataLCT_token_(consumes(ps.getParameter<edm::InputTag>("dataLCT"))), // check LCT in data
      dataALCTShower_token_(consumes(ps.getParameter<edm::InputTag>("dataALCTShower"))),
      dataALCTShower_TMB_token_(consumes(ps.getParameter<edm::InputTag>("dataALCTShower_TMB"))),
      emulALCTShower_token_(consumes(ps.getParameter<edm::InputTag>("emulALCTShower"))),
      dataCLCTShower_token_(consumes(ps.getParameter<edm::InputTag>("dataCLCTShower"))),
      emulCLCTShower_token_(consumes(ps.getParameter<edm::InputTag>("emulCLCTShower"))),
      dataLCTShower_token_(consumes(ps.getParameter<edm::InputTag>("dataLCTShower"))),
      emulLCTShower_token_(consumes(ps.getParameter<edm::InputTag>("emulLCTShower")))
      {
  edm::Service<TFileService> fs;

  debug_ = ps.getParameter<bool>("debug");
  wireDigiProducer_ = ps.getParameter<edm::InputTag>("CSCWireDigiProducer");
  compDigiProducer_ = ps.getParameter<edm::InputTag>("CSCComparatorDigiProducer");
  wire_token_ = consumes<CSCWireDigiCollection>(wireDigiProducer_);
  comp_token_ = consumes<CSCComparatorDigiCollection>(compDigiProducer_);
};

CSCShowerAnalyzer::~CSCShowerAnalyzer() {};

void CSCShowerAnalyzer::beginJob(){

  edm::Service<TFileService> fs;
  
  counters_ =      fs->make<TH1D>("Counters", "Counters", 15, 0,15);
  h_clctBx_data_nom_   =     fs->make<TH1D>("clctBx_data_norm", "CLCT nominal shower BX(Data)", 15, 0,15);
  h_clctBx_data_tight_ =     fs->make<TH1D>("clctBx_data_tight", "CLCT tight shower BX(Data)", 15, 0,15);
  h_clctBx_emul_nom_   =     fs->make<TH1D>("clctBx_emul_norm", "CLCT nominal shower BX(Emulation)", 15, 0,15);
  h_clctBx_emul_tight_ =     fs->make<TH1D>("clctBx_emul_tight", "CLCT tight shower BX(Emulation)", 15, 0,15);

  h_alctBx_data_nom_   =     fs->make<TH1D>("alctBx_data_norm", "ALCT nominal shower BX(Data)", 15, 0,15);
  h_alctBx_data_tight_ =     fs->make<TH1D>("alctBx_data_tight", "ALCT tight shower BX(Data)", 15, 0,15);
  h_alctBx_emul_nom_   =     fs->make<TH1D>("alctBx_emul_norm", "ALCT nominal shower BX(Emulation)", 15, 0,15);
  h_alctBx_emul_tight_ =     fs->make<TH1D>("alctBx_emul_tight", "ALCT tight shower BX(Emulation)", 15, 0,15);

  h_lctBx_data_nom_   =     fs->make<TH1D>("lctBx_data_norm", "LCT nominal shower BX(Data)", 15, 0,15);
  h_lctBx_data_tight_ =     fs->make<TH1D>("lctBx_data_tight", "LCT tight shower BX(Data)", 15, 0,15);
  h_lctBx_emul_nom_   =     fs->make<TH1D>("lctBx_emul_norm", "LCT nominal shower BX(Emulation)", 15, 0,15);
  h_lctBx_emul_tight_ =     fs->make<TH1D>("lctBx_emul_tight", "LCT tight shower BX(Emulation)", 15, 0,15);

  h_alct_Bx_data_v_emul   =  fs->make<TH2D>("alct_Bx_data_v_emul", "ALCT HMT BX data v emul,data>Nominal", 15, 0 ,15, 15,0,15);
  h_clct_Bx_data_v_emul   =  fs->make<TH2D>("clct_Bx_data_v_emul", "CLCT HMT BX data v emul,data>Nominal", 15, 0 ,15, 15,0,15);
  h_lct_Bx_data_v_emul    =  fs->make<TH2D>("lct_Bx_data_v_emul", "LCT HMT BX data v emul,data>Nominal", 15, 0 ,15, 15,0,15);

  h_alct_thres_data_v_emul=  fs->make<TH2D>("alct_thres_data_v_emul", "ALCT HMT thres data v emul", 3, 1 ,4, 3,1,4);
  h_clct_thres_data_v_emul=  fs->make<TH2D>("clct_thres_data_v_emul", "CLCT HMT thres data v emul", 3, 1 ,4, 3,1,4);
  h_lct_thres_data_v_emul =  fs->make<TH2D>("lct_thres_data_v_emul" , "LCT HMT thres data v emul" , 3, 1 ,4, 3,1,4);

  h_alct_nom_Bx_v_station =  fs->make<TH2D>("alct_nom_Bx_v_station", "Data Anode Nominal Shower", 15, 0, 15, 18, 0, 18);
  h_clct_nom_Bx_v_station =  fs->make<TH2D>("clct_nom_Bx_v_station", "Data Cathode Nominal Shower", 15, 0, 15, 18, 0, 18);
  h_lct_nom_Bx_v_station  =  fs->make<TH2D>("lct_nom_Bx_v_station" , "Data LCT Nominal Shower", 15, 0, 15, 18, 0, 18);
  h_alct_tight_Bx_v_station =  fs->make<TH2D>("alct_tight_Bx_v_station", "Data Anode Tight Shower", 15, 0, 15, 18, 0, 18);
  h_clct_tight_Bx_v_station =  fs->make<TH2D>("clct_tight_Bx_v_station", "Data Cathode Tight Shower", 15, 0, 15, 18, 0, 18);
  h_lct_tight_Bx_v_station  =  fs->make<TH2D>("lct_tight_Bx_v_station" , "Data LCT Tight Shower", 15, 0, 15, 18, 0, 18);


  lctShowerDataNomSummary_denom_ =
      fs->make<TH2D>("lct_cscshower_data_nom_summary_denom", "Data LCT Nominal Shower All", 36, 1, 37, 18, 0, 18);
  lctShowerDataNomSummary_num_ = fs->make<TH2D>(
      "lct_cscshower_data_nom_summary_num", "Data LCT Nominal Shower Emul Matched", 36, 1, 37, 18, 0, 18);
  alctShowerDataNomSummary_denom_ =
      fs->make<TH2D>("alct_cscshower_data_nom_summary_denom", "Data ALCT Nominal Shower All", 36, 1, 37, 18, 0, 18);
  alctShowerDataNomSummary_num_ = fs->make<TH2D>(
      "alct_cscshower_data_nom_summary_num", "Data ALCT Nominal Shower Emul Matched", 36, 1, 37, 18, 0, 18);
  clctShowerDataNomSummary_denom_ =
      fs->make<TH2D>("clct_cscshower_data_nom_summary_denom", "Data CLCT Nominal Shower All", 36, 1, 37, 18, 0, 18);
  clctShowerDataNomSummary_num_ = fs->make<TH2D>(
      "clct_cscshower_data_nom_summary_num", "Data CLCT Nominal Shower Emul Matched", 36, 1, 37, 18, 0, 18);

  lctShowerEmulNomSummary_denom_ =
      fs->make<TH2D>("lct_cscshower_emul_nom_summary_denom", "Emul LCT Nominal Shower All", 36, 1, 37, 18, 0, 18);
  lctShowerEmulNomSummary_num_ = fs->make<TH2D>(
      "lct_cscshower_emul_nom_summary_num", "Emul LCT Nominal Shower Not Matched to Data", 36, 1, 37, 18, 0, 18);
  alctShowerEmulNomSummary_denom_ =
      fs->make<TH2D>("alct_cscshower_emul_nom_summary_denom", "Emul ALCT Nominal Shower All", 36, 1, 37, 18, 0, 18);
  alctShowerEmulNomSummary_num_ = fs->make<TH2D>(
      "alct_cscshower_emul_nom_summary_num", "Emul ALCT Nominal Shower Not Matched to Data", 36, 1, 37, 18, 0, 18);
  clctShowerEmulNomSummary_denom_ =
      fs->make<TH2D>("clct_cscshower_emul_nom_summary_denom", "Emul CLCT Nominal Shower All", 36, 1, 37, 18, 0, 18);
  clctShowerEmulNomSummary_num_ = fs->make<TH2D>(
      "clct_cscshower_emul_nom_summary_num", "Emul CLCT Nominal Shower Not Matched to Data", 36, 1, 37, 18, 0, 18);

  lctShowerDataTightSummary_denom_ =
      fs->make<TH2D>("lct_cscshower_data_tight_summary_denom", "Data LCT Tight Shower All", 36, 1, 37, 18, 0, 18);
  lctShowerDataTightSummary_num_ = fs->make<TH2D>(
      "lct_cscshower_data_tight_summary_num", "Data LCT Tight Shower Emul Matched", 36, 1, 37, 18, 0, 18);
  alctShowerDataTightSummary_denom_ =
      fs->make<TH2D>("alct_cscshower_data_tight_summary_denom", "Data ALCT Tight Shower All", 36, 1, 37, 18, 0, 18);
  alctShowerDataTightSummary_num_ = fs->make<TH2D>(
      "alct_cscshower_data_tight_summary_num", "Data ALCT Tight Shower Emul Matched", 36, 1, 37, 18, 0, 18);
  clctShowerDataTightSummary_denom_ =
      fs->make<TH2D>("clct_cscshower_data_tight_summary_denom", "Data CLCT Tight Shower All", 36, 1, 37, 18, 0, 18);
  clctShowerDataTightSummary_num_ = fs->make<TH2D>(
      "clct_cscshower_data_tight_summary_num", "Data CLCT Tight Shower Emul Matched", 36, 1, 37, 18, 0, 18);

  lctShowerEmulTightSummary_denom_ =
      fs->make<TH2D>("lct_cscshower_emul_tight_summary_denom", "Emul LCT Tight Shower All", 36, 1, 37, 18, 0, 18);
  lctShowerEmulTightSummary_num_ = fs->make<TH2D>(
      "lct_cscshower_emul_tight_summary_num", "Emul LCT Tight Shower Not Matched to Data", 36, 1, 37, 18, 0, 18);
  alctShowerEmulTightSummary_denom_ =
      fs->make<TH2D>("alct_cscshower_emul_tight_summary_denom", "Emul ALCT Tight Shower All", 36, 1, 37, 18, 0, 18);
  alctShowerEmulTightSummary_num_ = fs->make<TH2D>(
      "alct_cscshower_emul_tight_summary_num", "Emul ALCT Tight Shower Not Matched to Data", 36, 1, 37, 18, 0, 18);
  clctShowerEmulTightSummary_denom_ =
      fs->make<TH2D>("clct_cscshower_emul_tight_summary_denom", "Emul CLCT Tight Shower All", 36, 1, 37, 18, 0, 18);
  clctShowerEmulTightSummary_num_ = fs->make<TH2D>(
      "clct_cscshower_emul_tight_summary_num", "Emul CLCT Tight Shower Not Matched to Data", 36, 1, 37, 18, 0, 18);

  // x labels
  h_alct_Bx_data_v_emul->GetXaxis()->SetTitle("Data Bx"); 
  h_clct_Bx_data_v_emul->GetXaxis()->SetTitle("Data Bx");  
  h_lct_Bx_data_v_emul->GetXaxis()->SetTitle( "Data Bx");    

  h_alct_thres_data_v_emul->GetXaxis()->SetTitle("Data");
  h_clct_thres_data_v_emul->GetXaxis()->SetTitle("Data");
  h_lct_thres_data_v_emul ->GetXaxis()->SetTitle("Data");

  h_alct_Bx_data_v_emul->GetYaxis()->SetTitle("Emulator BX"); 
  h_clct_Bx_data_v_emul->GetYaxis()->SetTitle("Emulator BX");  
  h_lct_Bx_data_v_emul ->GetYaxis()->SetTitle("Emulator BX");    

  h_alct_thres_data_v_emul->GetYaxis()->SetTitle("Emulator");
  h_clct_thres_data_v_emul->GetYaxis()->SetTitle("Emulator");
  h_lct_thres_data_v_emul ->GetYaxis()->SetTitle("Emulator");

  h_alct_nom_Bx_v_station->GetXaxis()->SetTitle("Bx");
  h_clct_nom_Bx_v_station->GetXaxis()->SetTitle("Bx");
  h_lct_nom_Bx_v_station->GetXaxis()->SetTitle("Bx");
  h_alct_tight_Bx_v_station->GetXaxis()->SetTitle("Bx");
  h_clct_tight_Bx_v_station->GetXaxis()->SetTitle("Bx");
  h_lct_tight_Bx_v_station->GetXaxis()->SetTitle("Bx");

  lctShowerDataNomSummary_denom_->GetXaxis()->SetTitle("Chamber");
  lctShowerDataNomSummary_num_->GetXaxis()->SetTitle("Chamber");
  alctShowerDataNomSummary_denom_->GetXaxis()->SetTitle("Chamber");
  alctShowerDataNomSummary_num_->GetXaxis()->SetTitle("Chamber");
  clctShowerDataNomSummary_denom_->GetXaxis()->SetTitle("Chamber");
  clctShowerDataNomSummary_num_->GetXaxis()->SetTitle("Chamber");

  lctShowerEmulNomSummary_denom_->GetXaxis()->SetTitle("Chamber");
  lctShowerEmulNomSummary_num_->GetXaxis()->SetTitle("Chamber");
  alctShowerEmulNomSummary_denom_->GetXaxis()->SetTitle("Chamber");
  alctShowerEmulNomSummary_num_->GetXaxis()->SetTitle("Chamber");
  clctShowerEmulNomSummary_denom_->GetXaxis()->SetTitle("Chamber");
  clctShowerEmulNomSummary_num_->GetXaxis()->SetTitle("Chamber");

  lctShowerDataTightSummary_denom_->GetXaxis()->SetTitle("Chamber");
  lctShowerDataTightSummary_num_->GetXaxis()->SetTitle("Chamber");
  alctShowerDataTightSummary_denom_->GetXaxis()->SetTitle("Chamber");
  alctShowerDataTightSummary_num_->GetXaxis()->SetTitle("Chamber");
  clctShowerDataTightSummary_denom_->GetXaxis()->SetTitle("Chamber");
  clctShowerDataTightSummary_num_->GetXaxis()->SetTitle("Chamber");

  lctShowerEmulTightSummary_denom_->GetXaxis()->SetTitle("Chamber");
  lctShowerEmulTightSummary_num_->GetXaxis()->SetTitle("Chamber");
  alctShowerEmulTightSummary_denom_->GetXaxis()->SetTitle("Chamber");
  alctShowerEmulTightSummary_num_->GetXaxis()->SetTitle("Chamber");
  clctShowerEmulTightSummary_denom_->GetXaxis()->SetTitle("Chamber");
  clctShowerEmulTightSummary_num_->GetXaxis()->SetTitle("Chamber");

  // plotting option
  h_alct_nom_Bx_v_station->SetOption("colz");
  h_clct_nom_Bx_v_station->SetOption("colz");
  h_lct_nom_Bx_v_station->SetOption("colz");
  h_alct_tight_Bx_v_station->SetOption("colz");
  h_clct_tight_Bx_v_station->SetOption("colz");
  h_lct_tight_Bx_v_station->SetOption("colz");

  lctShowerDataNomSummary_denom_->SetOption("colz");
  lctShowerDataNomSummary_num_->SetOption("colz");
  alctShowerDataNomSummary_denom_->SetOption("colz");
  alctShowerDataNomSummary_num_->SetOption("colz");
  clctShowerDataNomSummary_denom_->SetOption("colz");
  clctShowerDataNomSummary_num_->SetOption("colz");

  lctShowerEmulNomSummary_denom_->SetOption("colz");
  lctShowerEmulNomSummary_num_->SetOption("colz");
  alctShowerEmulNomSummary_denom_->SetOption("colz");
  alctShowerEmulNomSummary_num_->SetOption("colz");
  clctShowerEmulNomSummary_denom_->SetOption("colz");
  clctShowerEmulNomSummary_num_->SetOption("colz");

  lctShowerDataTightSummary_denom_->SetOption("colz");
  lctShowerDataTightSummary_num_->SetOption("colz");
  alctShowerDataTightSummary_denom_->SetOption("colz");
  alctShowerDataTightSummary_num_->SetOption("colz");
  clctShowerDataTightSummary_denom_->SetOption("colz");
  clctShowerDataTightSummary_num_->SetOption("colz");

  lctShowerEmulTightSummary_denom_->SetOption("colz");
  lctShowerEmulTightSummary_num_->SetOption("colz");
  alctShowerEmulTightSummary_denom_->SetOption("colz");
  alctShowerEmulTightSummary_num_->SetOption("colz");
  clctShowerEmulTightSummary_denom_->SetOption("colz");
  clctShowerEmulTightSummary_num_->SetOption("colz");

  const std::vector<std::string> suffix_label={"4/2", "4/1", "3/2", "3/1", " 2/2", "2/1", "1/3", "1/2", "1/1"};

  h_alct_thres_data_v_emul->GetXaxis()->SetBinLabel(1,"Loose");
  h_alct_thres_data_v_emul->GetXaxis()->SetBinLabel(2,"Nominal");
  h_alct_thres_data_v_emul->GetXaxis()->SetBinLabel(3,"Tight");
  h_clct_thres_data_v_emul->GetXaxis()->SetBinLabel(1,"Loose");
  h_clct_thres_data_v_emul->GetXaxis()->SetBinLabel(2,"Nominal");
  h_clct_thres_data_v_emul->GetXaxis()->SetBinLabel(3,"Tight");
  h_lct_thres_data_v_emul->GetXaxis()->SetBinLabel(1,"Loose");
  h_lct_thres_data_v_emul->GetXaxis()->SetBinLabel(2,"Nominal");
  h_lct_thres_data_v_emul->GetXaxis()->SetBinLabel(3,"Tight");

  h_alct_thres_data_v_emul->GetYaxis()->SetBinLabel(1,"Loose");
  h_alct_thres_data_v_emul->GetYaxis()->SetBinLabel(2,"Nominal");
  h_alct_thres_data_v_emul->GetYaxis()->SetBinLabel(3,"Tight");
  h_clct_thres_data_v_emul->GetYaxis()->SetBinLabel(1,"Loose");
  h_clct_thres_data_v_emul->GetYaxis()->SetBinLabel(2,"Nominal");
  h_clct_thres_data_v_emul->GetYaxis()->SetBinLabel(3,"Tight");
  h_lct_thres_data_v_emul->GetYaxis()->SetBinLabel(1,"Loose");
  h_lct_thres_data_v_emul->GetYaxis()->SetBinLabel(2,"Nominal");
  h_lct_thres_data_v_emul->GetYaxis()->SetBinLabel(3,"Tight");

  counters_->GetXaxis()->SetBinLabel(1, "ALCT loose") ;
  counters_->GetXaxis()->SetBinLabel(2, "ALCT loose&ALCT") ;
  counters_->GetXaxis()->SetBinLabel(3, "ALCT nominal") ;
  counters_->GetXaxis()->SetBinLabel(4, "ALCT nominal&ALCT") ;
  counters_->GetXaxis()->SetBinLabel(5, "ALCT nominal&LCT") ;
  counters_->GetXaxis()->SetBinLabel(6, "ALCT tight") ;
  counters_->GetXaxis()->SetBinLabel(7, "ALCT tight&ALCT") ;
  counters_->GetXaxis()->SetBinLabel(8, "CLCT nominal") ;
  counters_->GetXaxis()->SetBinLabel(9, "CLCT nominal&LCT") ;
  counters_->GetXaxis()->SetBinLabel(10,"CLCT tight") ;
  counters_->GetXaxis()->SetBinLabel(11,"CLCT tight&LCT") ;
  counters_->GetXaxis()->SetBinLabel(12,"LCT nominal") ;
  counters_->GetXaxis()->SetBinLabel(13,"LCT nominal&LCT") ;
  counters_->GetXaxis()->SetBinLabel(14,"LCT tight") ;
  counters_->GetXaxis()->SetBinLabel(15,"LCT tight&LCT") ;

  // y labels
  for (int ybin = 1; ybin <= 9; ++ybin) {
    h_alct_nom_Bx_v_station->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;
    h_clct_nom_Bx_v_station->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;
    h_lct_nom_Bx_v_station->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;
    h_alct_tight_Bx_v_station->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;
    h_clct_tight_Bx_v_station->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;
    h_lct_tight_Bx_v_station->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;

    lctShowerDataNomSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data()) ;
    lctShowerDataNomSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerDataNomSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerDataNomSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerDataNomSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerDataNomSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());

    lctShowerEmulNomSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    lctShowerEmulNomSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerEmulNomSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerEmulNomSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerEmulNomSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerEmulNomSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());


    h_alct_nom_Bx_v_station->GetYaxis()->SetBinLabel(19-ybin, (std::string("ME+") + suffix_label[ybin - 1]).data()) ;
    h_clct_nom_Bx_v_station->GetYaxis()->SetBinLabel(19-ybin, (std::string("ME+") + suffix_label[ybin - 1]).data()) ;
    h_lct_nom_Bx_v_station->GetYaxis()->SetBinLabel(19-ybin, (std::string("ME+") + suffix_label[ybin - 1]).data()) ;
    h_alct_tight_Bx_v_station->GetYaxis()->SetBinLabel(19-ybin, (std::string("ME+") + suffix_label[ybin - 1]).data()) ;
    h_clct_tight_Bx_v_station->GetYaxis()->SetBinLabel(19-ybin, (std::string("ME+") + suffix_label[ybin - 1]).data()) ;
    h_lct_tight_Bx_v_station->GetYaxis()->SetBinLabel(19-ybin, (std::string("ME+") + suffix_label[ybin - 1]).data()) ;

    lctShowerDataNomSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    lctShowerDataNomSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerDataNomSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerDataNomSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerDataNomSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerDataNomSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());

    lctShowerEmulNomSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    lctShowerEmulNomSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerEmulNomSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerEmulNomSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerEmulNomSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerEmulNomSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());

    lctShowerDataTightSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    lctShowerDataTightSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerDataTightSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerDataTightSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerDataTightSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerDataTightSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());

    lctShowerEmulTightSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    lctShowerEmulTightSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerEmulTightSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    alctShowerEmulTightSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerEmulTightSummary_denom_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());
    clctShowerEmulTightSummary_num_->GetYaxis()->SetBinLabel(ybin, (std::string("ME-") + suffix_label[ybin - 1]).data());

    lctShowerDataTightSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    lctShowerDataTightSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerDataTightSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerDataTightSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerDataTightSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerDataTightSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());

    lctShowerEmulTightSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    lctShowerEmulTightSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerEmulTightSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    alctShowerEmulTightSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerEmulTightSummary_denom_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
    clctShowerEmulTightSummary_num_->GetYaxis()->SetBinLabel(19 - ybin, (std::string("ME+") + suffix_label[ybin - 1]).data());
  }
}
void CSCShowerAnalyzer::analyze(const edm::Event& e, const edm::EventSetup& c) {
  // handles
  edm::Handle<CSCALCTDigiCollection>   dataALCT;
  edm::Handle<CSCCorrelatedLCTDigiCollection>   dataLCT;
  edm::Handle<CSCShowerDigiCollection> dataALCTshs;
  edm::Handle<CSCShowerDigiCollection> dataALCT_TMBshs;
  edm::Handle<CSCShowerDigiCollection> emulALCTshs;
  edm::Handle<CSCShowerDigiCollection> dataCLCTshs;
  edm::Handle<CSCShowerDigiCollection> emulCLCTshs;
  edm::Handle<CSCShowerDigiCollection> dataLCTshs;
  edm::Handle<CSCShowerDigiCollection> emulLCTshs;

  edm::Handle<CSCComparatorDigiCollection> compDigis;
  edm::Handle<CSCWireDigiCollection> wireDigis;
  //edm::Handle<CSCWireDigi> wireDigis;

  e.getByToken(dataALCT_token_, dataALCT);
  e.getByToken(dataLCT_token_, dataLCT);
  e.getByToken(dataALCTShower_token_, dataALCTshs);
  e.getByToken(dataALCTShower_TMB_token_, dataALCT_TMBshs);
  e.getByToken(emulALCTShower_token_, emulALCTshs);
  e.getByToken(dataCLCTShower_token_, dataCLCTshs);
  e.getByToken(emulCLCTShower_token_, emulCLCTshs);
  e.getByToken(dataLCTShower_token_, dataLCTshs);
  e.getByToken(emulLCTShower_token_, emulLCTshs);

  e.getByToken(comp_token_, compDigis);
  e.getByToken(wire_token_, wireDigis);

  const std::map<std::pair<int, int>, int> histIndexCSC = {{{1, 1}, 8},
                                                           {{1, 2}, 7},
                                                           {{1, 3}, 6},
                                                           {{2, 1}, 5},
                                                           {{2, 2}, 4},
                                                           {{3, 1}, 3},
                                                           {{3, 2}, 2},
                                                           {{4, 1}, 1},
                                                           {{4, 2}, 0}};

  const int min_endcap = CSCDetId::minEndcapId();
  const int max_endcap = CSCDetId::maxEndcapId();
  const int min_station = CSCDetId::minStationId();
  const int max_station = CSCDetId::maxStationId();
  const int min_sector = CSCTriggerNumbering::minTriggerSectorId();
  const int max_sector = CSCTriggerNumbering::maxTriggerSectorId();
  const int min_subsector = CSCTriggerNumbering::minTriggerSubSectorId();
  const int max_subsector = CSCTriggerNumbering::maxTriggerSubSectorId();
  const int min_chamber = CSCTriggerNumbering::minTriggerCscId();
  const int max_chamber = CSCTriggerNumbering::maxTriggerCscId();

  int nALCT_showers= 0;
  for (int endc = min_endcap; endc <= max_endcap; endc++) {
    // loop on all stations
    for (int stat = min_station; stat <= max_station; stat++) {
      int numsubs = ((stat == 1) ? max_subsector : 1);
      // loop on sectors and subsectors
      for (int sect = min_sector; sect <= max_sector; sect++) {
        for (int subs = min_subsector; subs <= numsubs; subs++) {
          // loop on all chambers
          for (int cham = min_chamber; cham <= max_chamber; cham++) {
            // extract the ring number
            int ring = CSCTriggerNumbering::ringFromTriggerLabels(stat, cham);

            // actual chamber number =/= trigger chamber number
            int chid = CSCTriggerNumbering::chamberFromTriggerLabels(sect, subs, stat, cham);

            // 0th layer means whole chamber.
            CSCDetId detid(endc, stat, ring, chid, 0);

            int chamber = detid.chamber();

            //CSCMotherboard* tmb = tmb_[endc - 1][stat - 1][sect - 1][subs - 1][cham - 1].get();

            //tmb->setCSCGeometry(csc_g);
            //tmb->run(wiredc, compdc);

            int sr = histIndexCSC.at({stat, ring});
            if (endc == 1)
              sr = 17 - sr;


            // ALCT analysis (TMB ALCT bit input)
            auto range_dataALCT    =   dataALCT->get(detid);
            auto range_dataLCT    =   dataLCT->get(detid);
            auto range_dataALCTshs = dataALCTshs->get(detid);
            auto range_dataALCT_TMBshs = dataALCT_TMBshs->get(detid);
            auto range_emulALCT    = emulALCTshs->get(detid);

            bool has_alct = false;
            bool has_lct = false;
            //bool has_ealct_tight=false;
            //bool has_ealct_nom=false;

            //bool has_dalct_tight=false;
            //bool has_dalct_nom=false;
            int nMatched_ealct=0;
            // check TMB showers
            for (auto dalct = range_dataALCT_TMBshs.first; dalct != range_dataALCT_TMBshs.second; dalct++) {
                for (auto ealct = range_emulALCT.first; ealct != range_emulALCT.second; ealct++) {
                    if (dalct->isNominalInTime()) h_alct_Bx_data_v_emul->Fill(dalct->getBX(),ealct->getBX());
                    h_alct_thres_data_v_emul->Fill(dalct->bitsInTime(),ealct->bitsInTime());
                }
            }
           // check ALCT
            for (auto dalct_trk = range_dataALCT.first; dalct_trk != range_dataALCT.second; dalct_trk++) {
              if (dalct_trk->isValid()){ 
                  has_alct= true;
              }
            }
           // check LCT
            for (auto dlct_trk = range_dataLCT.first; dlct_trk != range_dataLCT.second; dlct_trk++) {
              if (dlct_trk->isValid()) has_lct=true;
            }

            for (auto dalct = range_dataALCT_TMBshs.first; dalct != range_dataALCT_TMBshs.second; dalct++) {
              if (dalct->isValid()) counters_->Fill("ALCT loose",1);
              if (dalct->isValid() and has_alct) counters_->Fill("ALCT loose&ALCT",1);
              if (dalct->isValid() and dalct->isNominalInTime() ) counters_->Fill("ALCT nominal",1);
              if (dalct->isValid() and dalct->isTightInTime() ) counters_->Fill("ALCT tight",1);
              if (dalct->isValid() and dalct->isNominalInTime() ) {
                if (has_alct) counters_->Fill("ALCT nominal&ALCT",1);
                if (has_lct) counters_->Fill("ALCT nominal&LCT",1);
                h_alct_nom_Bx_v_station->Fill(dalct->getBX(),sr);
                h_alctBx_data_nom_->Fill(dalct->getBX());
                if (dalct->isTightInTime()) {
                  if (has_alct) counters_->Fill("ALCT tight&ALCT",1);
                  if (has_lct) counters_->Fill("ALCT tight&LCT",1);
                  fillhist(alctShowerDataTightSummary_denom_, chamber,sr);
                  h_alctBx_data_tight_->Fill(dalct->getBX());
                  h_alct_tight_Bx_v_station->Fill(dalct->getBX(),sr);
                  //has_dalct_tight=true;
                }
                //has_dalct_nom=true;
                fillhist(alctShowerDataNomSummary_denom_, chamber,sr);
                // check for least one matching ALCT
                for (auto ealct = range_emulALCT.first; ealct != range_emulALCT.second; ealct++) {
                  if (ealct->isTightInTime()){ h_alctBx_emul_tight_->Fill(ealct->getBX());}
                  if (ealct->isNominalInTime()){ h_alctBx_emul_nom_->Fill(ealct->getBX());}
                  if (ealct->isValid() and areSameShowers(*dalct, *ealct)) {
                    if (dalct->isTightInTime()) {
                      fillhist(alctShowerDataTightSummary_num_, chamber,sr);
                      h_alctBx_emul_tight_->Fill(ealct->getBX());
                      //has_ealct_tight=true;
                    }
                    fillhist(alctShowerDataNomSummary_num_, chamber,sr);
                    //has_ealct_nom=true;
                    nMatched_ealct+=1;  
                  }
                }
              }
            }  // End of for (auto dalct = range_dataALCT.first; dalct != range_dataALCT.second; dalct++)
            // printout
            if (debug_){
                //if (( has_dalct_nom and !has_ealct_nom) or (has_dalct_tight and !has_ealct_tight)){
                //    for (auto dalct = range_dataALCTshs.first; dalct != range_dataALCTshs.second; dalct++) {
                //        std::cout<< " Data ALCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //        std::cout<< " alct (isNominal,isTight)= ("<< dalct->isNominalInTime() <<","<<dalct->isTightInTime()<<")";
                //        std::cout<< " bits=" <<(*dalct) <<std::endl;
                //    }
                //    for (auto ealct = range_emulALCT.first; ealct != range_emulALCT.second; ealct++) {
                //        std::cout<< " Emul ALCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //        std::cout<< " alct (isNominal,isTight)= ("<< ealct->isNominalInTime() <<","<<ealct->isTightInTime()<<")";
                //        std::cout<< " bits=" <<(*ealct) <<std::endl;
                //    }
                //    for (auto dalct = range_dataALCT_TMBshs.first; dalct != range_dataALCT_TMBshs.second; dalct++) {
                //        std::cout<< " Data ALCT[TMB]: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //        std::cout<< " alct (isNominal,isTight)= ("<< dalct->isNominalInTime() <<","<<dalct->isTightInTime()<<")";
                //        std::cout<< " bits=" <<(*dalct) <<std::endl;
                //    }
                //    int nHits_Bx8 = 0;
                //    int nHits_BxAll = 0;
                //    int nLayersWithHit = 0;
                //    for (int ilayer = CSCDetId::minLayerId(); ilayer <= CSCDetId::maxLayerId(); ++ilayer) {
                //        bool hasBX8hit = false;
                //        CSCDetId wireId(endc, stat, ring, chid, ilayer);
                //        auto range_wireDigis   = wireDigis->get(wireId);
                //        for(auto wire = range_wireDigis.first; wire!=range_wireDigis.second;wire++){
                //            std::vector<int> tbins = wire->getTimeBinsOn();
                //            for (auto & tbin : tbins) { 
                //                if (tbin==8){
                //                     nHits_Bx8++;
                //                     hasBX8hit=true;
                //                }
                //            }
                //            nHits_BxAll++;
                //            std::cout<<"  Layer= "<<ilayer<< "  wires= " <<(*wire) <<std::endl;
                //        }    
                //        if (hasBX8hit) nLayersWithHit++;
                //    }
                //    std::cout<<"  nHits Bx8= "<<nHits_Bx8<< "  nHits_BxAll = " <<nHits_BxAll <<" nLayersWithBX8Hit = "<< nLayersWithHit<<std::endl;
                //  for (auto dalct_trk = range_dataALCT.first; dalct_trk != range_dataALCT.second; dalct_trk++) {
                //        std::cout<<"    ALCT_trk=" <<(*dalct_trk)<<std::endl;
                //  }
                //}
            }


            for (auto ealct = range_emulALCT.first; ealct != range_emulALCT.second; ealct++) {
              bool isMatched = false;
              if (ealct->isValid() and ealct->isNominalInTime()) {
                if (ealct->isTightInTime()) {
                  fillhist(alctShowerEmulTightSummary_denom_, chamber,sr);
                }
                fillhist(alctShowerEmulNomSummary_denom_, chamber,sr);
                for (auto dalct = range_dataALCT_TMBshs.first; dalct != range_dataALCT_TMBshs.second; dalct++) {
                  if (areSameShowers(*dalct, *ealct) )
                    isMatched = true;
                }
                // only fill when it is not matched to an ALCT
                // to understand if the emulator is producing too many ALCTs
                //if (!isMatched) 
                if (!isMatched && has_lct) { // filter out mismatches without lct
                  if (ealct->isTightInTime()) {
                    fillhist(alctShowerEmulTightSummary_num_, chamber,sr);
                  }
                  fillhist(alctShowerEmulNomSummary_num_, chamber,sr);
                  if (debug_){
                      for (auto dalct = range_dataALCTshs.first; dalct != range_dataALCTshs.second; dalct++) {
                          std::cout<< " Data ALCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                          std::cout<< " alct (isNominal,isTight)= ("<< dalct->isNominalInTime() <<","<<dalct->isTightInTime()<<")";
                          std::cout<< " bits=" <<(*dalct) <<std::endl;
                      }
                          std::cout<< " Emul ALCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                          std::cout<< " alct (isNominal,isTight)= ("<< ealct->isNominalInTime() <<","<<ealct->isTightInTime()<<")";
                          std::cout<< " bits=" <<(*ealct) <<std::endl;
                      for (auto dalct = range_dataALCT_TMBshs.first; dalct != range_dataALCT_TMBshs.second; dalct++) {
                          std::cout<< " Data ALCT[TMB]: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                          std::cout<< " alct (isNominal,isTight)= ("<< dalct->isNominalInTime() <<","<<dalct->isTightInTime()<<")";
                          std::cout<< " bits=" <<(*dalct) <<std::endl;
                      }
                    int nHits_Bx8 = 0;
                    int nHits_BxAll = 0;
                    int nLayersWithHit = 0;
                      for (int ilayer = CSCDetId::minLayerId(); ilayer <= CSCDetId::maxLayerId(); ++ilayer) {
                          bool hasBX8hit = false;
                          CSCDetId wireId(endc, stat, ring, chid, ilayer);
                          auto range_wireDigis   = wireDigis->get(wireId);
                          for(auto wire = range_wireDigis.first; wire!=range_wireDigis.second;wire++){
                              std::vector<int> tbins = wire->getTimeBinsOn();
                              for (auto & tbin : tbins) { 
                                  if (tbin==8){
                                       nHits_Bx8++;
                                       hasBX8hit=true;
                                  }
                              }
                              nHits_BxAll++;
                              std::cout<<"  Layer= "<<ilayer<< "  wires= " <<(*wire) <<std::endl;
                          }    
                          if (hasBX8hit) nLayersWithHit++;
                      }
                      std::cout<<"  nHits Bx8= "<<nHits_Bx8<< "  nHits_BxAll = " <<nHits_BxAll <<" nLayersWithBX8Hit = "<< nLayersWithHit<<std::endl;
                      for (auto dalct_trk = range_dataALCT.first; dalct_trk != range_dataALCT.second; dalct_trk++) {
                            std::cout<<"    ALCT_trk=" <<(*dalct_trk)<<std::endl;
                      }
                      for (auto dlct_trk = range_dataLCT.first; dlct_trk != range_dataLCT.second; dlct_trk++) {
                            std::cout<<"    LCT_trk=" <<(*dlct_trk)<<std::endl;
                      }
                    }
                  }
                }
            }  // End of for (auto ealct = range_emulALCT.first; ealct != range_emulALCT.second; ealct++)

            // CLCT analysis
            auto range_dataCLCT = dataCLCTshs->get(detid);
            auto range_emulCLCT = emulCLCTshs->get(detid);

            for (auto dclct = range_dataCLCT.first; dclct != range_dataCLCT.second; dclct++) {
                for (auto eclct = range_emulCLCT.first; eclct != range_emulCLCT.second; eclct++) {
                    if (dclct->isNominalInTime()) h_clct_Bx_data_v_emul->Fill(dclct->getBX(),eclct->getBX());
                    h_clct_thres_data_v_emul->Fill(dclct->bitsInTime(),eclct->bitsInTime());
                }
            }

            for (auto dclct = range_dataCLCT.first; dclct != range_dataCLCT.second; dclct++) {
              if (dclct->isValid() and dclct->isNominalInTime() ) counters_->Fill("CLCT nominal",1);
              if (dclct->isValid() and dclct->isTightInTime() ) counters_->Fill("CLCT tight",1);
              if (dclct->isValid() and dclct->isNominalInTime()) {
                bool found_eclct = false;
                //    std::cout<< " Data CLCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //    std::cout<< " clct (isNominal,isTight)= ("<< dclct->isNominalInTime() <<","<<dclct->isTightInTime()<<")";
                //    std::cout<< " bits=" <<(*dclct) <<std::endl;
                h_clctBx_data_nom_->Fill(dclct->getBX());
                h_clct_nom_Bx_v_station->Fill(dclct->getBX(),sr);
                if (has_lct) counters_->Fill("CLCT nominal&LCT",1);
                if (dclct->isTightInTime()) {
                  if (has_lct) counters_->Fill("CLCT tight&LCT",1);
                  fillhist(clctShowerDataTightSummary_denom_, chamber,sr);
                  h_clctBx_data_tight_->Fill(dclct->getBX());
                  h_clct_tight_Bx_v_station->Fill(dclct->getBX(),sr);
                }
                fillhist(clctShowerDataNomSummary_denom_, chamber,sr);
                // check for least one matching CLCT
                for (auto eclct = range_emulCLCT.first; eclct != range_emulCLCT.second; eclct++) {
                //    std::cout<< " Emul CLCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //    std::cout<< " clct (isNominal,isTight)= ("<< eclct->isNominalInTime() <<","<<eclct->isTightInTime()<<")";
                //    std::cout<< " bits=" <<(*eclct) <<" areSameShower= "<<areSameShowers(*dclct, *eclct,false) <<std::endl;

                  if (eclct->isTightInTime()) {h_clctBx_emul_tight_->Fill(eclct->getBX());}
                  if (eclct->isNominalInTime()) {h_clctBx_emul_nom_->Fill(eclct->getBX());}
                  if (eclct->isValid() and areSameShowers(*dclct, *eclct,false)) {
                    if (dclct->isTightInTime()) {
                      found_eclct =true;
                      fillhist(clctShowerDataTightSummary_num_, chamber,sr);
                    }
                    found_eclct =true;
                    fillhist(clctShowerDataNomSummary_num_, chamber,sr);
                  }
                }
                // print comparator digi
                if (not found_eclct){
                    for (int ilayer = CSCDetId::minLayerId(); ilayer <= CSCDetId::maxLayerId(); ++ilayer) {
                        CSCDetId compId(endc, stat, ring, chid, ilayer);
                        auto range_compDigis   = compDigis->get(compId);
                        for(auto comp = range_compDigis.first; comp!=range_compDigis.second;comp++){
                            std::cout<<"  Layer= "<<ilayer<< "  comp= " <<(*comp) <<std::endl;
                        }   
                    } 
                }
              }
            }  // End of for (auto dclct = range_dataCLCT.first; dclct != range_dataCLCT.second; dclct++)

            for (auto eclct = range_emulCLCT.first; eclct != range_emulCLCT.second; eclct++) {
              bool isMatched = false;
              if (eclct->isValid() and eclct->isNominalInTime()) {
                if (eclct->isTightInTime()) {
                  fillhist(clctShowerEmulTightSummary_denom_, chamber,sr);
                }
                fillhist(clctShowerEmulNomSummary_denom_, chamber,sr);
                // check for least one matching CLCT
                for (auto dclct = range_dataCLCT.first; dclct != range_dataCLCT.second; dclct++) {
                  if (areSameShowers(*dclct, *eclct,false))
                    isMatched = true;
                }
                // only fill when it is not matched to an CLCT
                // to understand if the emulator is producing too many CLCTs
                if (!isMatched && has_lct) {
                  if (eclct->isTightInTime()) {
                    fillhist(clctShowerEmulTightSummary_num_, chamber,sr);
                  }
                  fillhist(clctShowerEmulNomSummary_num_, chamber,sr);
                }
              }
            }  // End of for (auto eclct = range_emulCLCT.first; eclct != range_emulCLCT.second; eclct++)

            // LCT analysis
            auto range_dataLCTshs = dataLCTshs->get(detid);
            auto range_emulLCT = emulLCTshs->get(detid);

            //bool has_dlct_nom=false; 
            //bool has_elct_nom=false;
            //bool has_dlct_tight=false ;
            //bool has_elct_tight=false ;

            //for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++) {
            //    for (auto elct = range_emulLCT.first; elct != range_emulLCT.second; elct++) {
            //        if (dlct->isNominalInTime()) h_lct_Bx_data_v_emul->Fill(dlct->getBX(),elct->getBX());
            //        h_lct_thres_data_v_emul->Fill(dlct->bitsInTime(),elct->bitsInTime());
            //    }
            //}

            //for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++) {
            //  if (dlct->isValid() and dlct->isNominalInTime() ) counters_->Fill("LCT nominal",1);
            //  if (dlct->isValid() and dlct->isTightInTime() ) counters_->Fill("LCT tight",1);
            //  if (dlct->isValid() and dlct->isNominalInTime()) {
            //    if (has_lct) counters_->Fill("LCT nominal&LCT",1);
            //    h_lctBx_data_nom_->Fill(dlct->getBX());
            //    if (dlct->isTightInTime()) {
            //      if (has_lct) counters_->Fill("LCT tight&LCT",1);
            //      h_lctBx_data_tight_->Fill(dlct->getBX());
            //      fillhist(lctShowerDataTightSummary_denom_, chamber,sr);
            //      h_lct_tight_Bx_v_station->Fill(dlct->getBX(),sr);
            //      has_dlct_tight = true;
            //    }
            //    has_dlct_nom=true;
            //    h_lct_nom_Bx_v_station->Fill(dlct->getBX(),sr);
            //    fillhist(lctShowerDataNomSummary_denom_, chamber,sr);
            //    // check for least one matching LCT
            //    for (auto elct = range_emulLCT.first; elct != range_emulLCT.second; elct++) {

            //      if (elct->isTightInTime()){ h_lctBx_emul_tight_->Fill(elct->getBX());}
            //      if (elct->isNominalInTime()){ h_lctBx_emul_nom_->Fill(elct->getBX());}
            //      if (elct->isValid() and areSameShowers(*dlct, *elct)) {
            //        if (dlct->isTightInTime()) {
            //          fillhist(lctShowerDataTightSummary_num_, chamber,sr);
            //          has_elct_tight = true;
            //        }
            //        fillhist(lctShowerDataNomSummary_num_, chamber,sr);
            //        has_elct_nom = true;
            //      }
            //    }
            //  }
            //}  // End of for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++)

            //has_dlct_nom =true;
            //has_elct_nom = true;
            //has_dlct_tight=true;
            //has_elct_tight=true;
            //printout mismatch LCT
            if (debug_){
                for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++) {
                  std::cout<< " Data matchLCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                  std::cout<< " mlct (isNominal,isTight)= ("<< dlct->isNominalInTime() <<","<<dlct->isTightInTime()<<")";
                  std::cout<< " bits=" <<(*dlct) <<std::endl;
                }
                //for (auto elct = range_emulLCT.first; elct != range_emulLCT.second; elct++) {
                //  std::cout<< " Emul matchLCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //  std::cout<< " emlct (isNominal,isTight)= ("<< elct->isNominalInTime() <<","<<elct->isTightInTime()<<")";
                //  std::cout<< " bits=" <<(*elct) <<std::endl;
                //}
                for (auto ealct = range_emulALCT.first; ealct != range_emulALCT.second; ealct++) {
                    std::cout<< " Emul ALCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                    std::cout<< " alct (isNominal,isTight)= ("<< ealct->isNominalInTime() <<","<<ealct->isTightInTime()<<")";
                    std::cout<< " bits=" <<(*ealct) <<std::endl;
                }
                for (auto dalct = range_dataALCT_TMBshs.first; dalct != range_dataALCT_TMBshs.second; dalct++) {
                    std::cout<< " Data ALCT[TMB]: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                    std::cout<< " alct (isNominal,isTight)= ("<< dalct->isNominalInTime() <<","<<dalct->isTightInTime()<<")";
                    std::cout<< " bits=" <<(*dalct) <<std::endl;
                }
                //for (auto eclct = range_emulCLCT.first; eclct != range_emulCLCT.second; eclct++) {
                //    std::cout<< " Emul CLCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                //    std::cout<< " clct (isNominal,isTight)= ("<< eclct->isNominalInTime() <<","<<eclct->isTightInTime()<<")";
                //    std::cout<< " bits=" <<(*eclct) <<std::endl;
                //}
                for (auto dclct = range_dataCLCT.first; dclct != range_dataCLCT.second; dclct++) {
                    std::cout<< " Data CLCT: (endcap,station,ring,chamber) = ("<< endc<<","<<stat<< ","<< ring<< ","<< chamber << ") ";
                    std::cout<< " clct (isNominal,isTight)= ("<< dclct->isNominalInTime() <<","<<dclct->isTightInTime()<<")";
                    std::cout<< " bits=" <<(*dclct) <<std::endl;
                }
               // for (int ilayer = CSCDetId::minLayerId(); ilayer <= CSCDetId::maxLayerId(); ++ilayer) {
               //     CSCDetId compId(endc, stat, ring, chid, ilayer);
               //     auto range_compDigis   = compDigis->get(compId);
               //     for(auto comp = range_compDigis.first; comp!=range_compDigis.second;comp++){
               //         std::cout<<"ID = "<< compId.chamberName()<<"  Layer= "<<ilayer<< "  comp= " <<(*comp) <<std::endl;
               //     }   
               // } 
                for (int ilayer = CSCDetId::minLayerId(); ilayer <= CSCDetId::maxLayerId(); ++ilayer) {
                    CSCDetId wireId(endc, stat, ring, chid, ilayer);
                    auto range_wireDigis   = wireDigis->get(wireId);
                    for(auto wire = range_wireDigis.first; wire!=range_wireDigis.second;wire++){
                        std::vector<int> tbins = wire->getTimeBinsOn();
                        std::cout<<"ID = "<<wireId.chamberName()<<"  Layer= "<<ilayer<< "  wires= " <<(*wire) <<std::endl;
                    }    
                }
              //for (auto dalct_trk = range_dataALCT.first; dalct_trk != range_dataALCT.second; dalct_trk++) {
              //      std::cout<<"    ALCT_trk=" <<(*dalct_trk)<<std::endl;
              //}
              //for (auto dlct_trk = range_dataLCT.first; dlct_trk != range_dataLCT.second; dlct_trk++) {
              //      std::cout<<"    LCT_trk=" <<(*dlct_trk)<<std::endl;
              //}
            }

            for (auto elct = range_emulLCT.first; elct != range_emulLCT.second; elct++) {
              bool isMatched = false;
              if (elct->isValid() and elct->isNominalInTime()) {
                if (elct->isTightInTime()) {
                  fillhist(lctShowerEmulTightSummary_denom_, chamber,sr);
                }
                fillhist(lctShowerEmulNomSummary_denom_, chamber,sr);
                // check for least one matching LCT
                for (auto dlct = range_dataLCTshs.first; dlct != range_dataLCTshs.second; dlct++) {
                  if (areSameShowers(*dlct, *elct))
                    isMatched = true;
                }
                // only fill when it is not matched to an LCT
                // to understand if the emulator is producing too many LCTs
                if (!isMatched && has_lct) {
                  if (elct->isTightInTime()) {
                    fillhist(lctShowerEmulTightSummary_num_, chamber,sr);
                  }
                  fillhist(lctShowerEmulNomSummary_num_, chamber,sr);
                }
              }
            }  // End of for (auto elct = range_emulLCT.first; elct != range_emulLCT.second; elct++) 

          }
        }
      }
    }
  }
};
void CSCShowerAnalyzer::fillhist(TH2D* h,int chamber,int sr) const{
    bool chamber20 = (sr == 1 or sr == 3 or sr == 5 or sr == 12 or sr == 14 or sr == 16);
     if (chamber20) {
       h->Fill(chamber * 2 - 1, sr, 0.5);
       h->Fill(chamber * 2, sr, 0.5);
     } else
       h->Fill(chamber, sr);
    return;
}
bool CSCShowerAnalyzer::areSameShowers(const CSCShowerDigi& dataShower, const CSCShowerDigi& rhs) const {
  return areSameShowers(dataShower,rhs,true);
}
bool CSCShowerAnalyzer::areSameShowers(const CSCShowerDigi& dataShower, const CSCShowerDigi& rhs, bool isALCT) const {
  bool returnValue = false;
  int dataBXshift = 3;
  if (!isALCT) {dataBXshift=2;
    if (dataShower.isValid() == rhs.isValid()  && dataShower.bitsInTime() == rhs.bitsInTime() &&
        dataShower.bitsOutOfTime() == rhs.bitsOutOfTime() && dataShower.getBX()+dataBXshift == rhs.getBX()) {
      returnValue = true;
    }
  }else{
    if (dataShower.isValid() == rhs.isValid()  && dataShower.bitsInTime() == rhs.bitsInTime() &&
        dataShower.bitsOutOfTime() == rhs.bitsOutOfTime() && dataShower.getBX()+dataBXshift == rhs.getBX()) {
      returnValue = true;
    }
  }
  return returnValue;
}
#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(CSCShowerAnalyzer);
