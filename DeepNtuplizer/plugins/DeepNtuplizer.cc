// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

//ROOT includes
#include "TTree.h"
#include <TFile.h>
#include <TROOT.h>
#include "TBranch.h"
#include <string>
#include <vector>
#include "TSystem.h"

//CMSSW includes
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

class DeepNtuplizer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit DeepNtuplizer(const edm::ParameterSet&);
  ~DeepNtuplizer();

  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);


private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;

  // ----------member data --------------------------- 
  edm::EDGetTokenT<reco::VertexCollection> vtxToken_;
  edm::EDGetTokenT<pat::JetCollection>     jetToken_;

  TFile *file_ = new TFile("output.root","recreate");
  TTree *tree_ = new TTree("tree","tree");

  unsigned int npv_;

  // jet variables
  float jet_pt_;

};


DeepNtuplizer::DeepNtuplizer(const edm::ParameterSet& iConfig):
  vtxToken_(consumes<reco::VertexCollection>(iConfig.getParameter<edm::InputTag>("vertices"))),
  jetToken_(consumes<pat::JetCollection>(iConfig.getParameter<edm::InputTag>("jets")))
{
  //now do what ever initialization is needed
  usesResource("TFileService");

  tree_->Branch("npv"    ,&npv_    ,"npv/i"    );

  // jet variables
  tree_->Branch("jet_pt", &jet_pt_);
}


DeepNtuplizer::~DeepNtuplizer()
{
  file_->Close();
}


// ------------ method called for each event  ------------
void
DeepNtuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
  edm::Handle<reco::VertexCollection> vertices;
  iEvent.getByToken(vtxToken_, vertices);
  if (vertices->empty()) return; // skip the event if no PV found
  const reco::Vertex &PV = vertices->front();

  edm::Handle<pat::JetCollection> jets;
  iEvent.getByToken(jetToken_, jets);

  // clear vectors
  npv_ = vertices->size();


  // loop over the jets
  for (const pat::Jet &jet : *jets) {
    jet_pt_ = jet.pt();
		tree_->Fill();
  }
}


// ------------ method called once each job just before starting event loop  ------------
void
DeepNtuplizer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
DeepNtuplizer::endJob()
{
  file_->cd();
  tree_->Write();
  file_->Write();
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DeepNtuplizer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DeepNtuplizer);
