#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"

// NON COMPILARE QUESTO PROGRAMMA SENZA AVER PRIMA ACCESO MOBA

int analyse() {

  gStyle->SetOptStat(10);
  // gStyle->SetOptFit(1111);

  // prendi i file
  TFile *simu_ON_file = new TFile("main142_ON_1e6.root", "READ");
  TFile *simu_OFF_file = new TFile("main142_OFF_1e6.root", "READ");
  TFile *root_file = new TFile("cern_data/proton_pt.root", "READ");
  // root_file->cd();

  // prendi gli istogrammi
  TH1D *h_pt_simu_ON_tot_pp = (TH1D *)simu_ON_file->Get("h_pt_tot_pp");
  TH1D *h_pt_simu_ON_proton = (TH1D *)simu_ON_file->Get("h_pt_proton");
  TH1D *h_pt_simu_ON_antiproton = (TH1D *)simu_ON_file->Get("h_pt_antiproton");
  TH1D *h_pt_simu_OFF_tot_pp = (TH1D *)simu_OFF_file->Get("h_pt_tot_pp");
  //TH1D *h_pt_cern_pp = (TH1D *)root_file->Get("Table 6/Hist1D_y1");
  TGraphAsymmErrors *h_pt_cern_pp = (TGraphAsymmErrors *)root_file->Get("Table 6/Graph1D_y1");

  //TCanvas *pp_canvas = new TCanvas("pp_pt", "pp momentum distribution", 1440, 720);
  TCanvas *pp_canvas = new TCanvas("pp_pt", "pp momentum distribution", 1440, 720);
  // pp_canvas->cd();
  pp_canvas->SetLogx();  // log x
  pp_canvas->SetLogy();  // log y
  pp_canvas->SetGridx(); // griglia x
  pp_canvas->SetGridy(); // griglia y

  // cosmetica cern
  h_pt_cern_pp->SetMarkerStyle(20);
  h_pt_cern_pp->SetLineColor(kBlack);
  h_pt_cern_pp->SetMarkerSize(1.2);
  
  // cosmetica simu on pp
  h_pt_simu_ON_tot_pp->SetMarkerStyle(22);
  h_pt_simu_ON_tot_pp->SetLineColor(kBlue);
  h_pt_simu_ON_tot_pp->SetMarkerColor(kBlue);
  h_pt_simu_ON_tot_pp->SetMarkerSize(1.4);

  // fare cosmetica
  h_pt_simu_ON_proton->SetMarkerStyle(8);
  h_pt_simu_ON_proton->SetLineColor(kRed);
  h_pt_simu_ON_proton->SetMarkerColor(kRed);

  h_pt_simu_ON_antiproton->SetMarkerStyle(4);
  h_pt_simu_ON_antiproton->SetLineColor(kViolet+7);
  h_pt_simu_ON_antiproton->SetMarkerColor(kViolet+7);

  // cosmetica simu off
  h_pt_simu_OFF_tot_pp->SetMarkerStyle(21);
  h_pt_simu_OFF_tot_pp->SetMarkerColor(kViolet);
  h_pt_simu_OFF_tot_pp->SetLineColor(kViolet);
  h_pt_simu_OFF_tot_pp->SetMarkerSize(1.2);

  // disegna i grafici sullo stesso canvas
  h_pt_simu_ON_tot_pp->Draw("E, P, SAME");
  h_pt_cern_pp->Draw("E, P, SAME");
  h_pt_simu_OFF_tot_pp->Draw("E, P, SAME");
  h_pt_simu_ON_tot_pp->Draw("E, P, SAME");
  h_pt_simu_ON_proton->Draw("E, P, SAME");
  h_pt_simu_ON_antiproton->Draw("E, P, SAME");

  // aggiungi la legenda
  TLegend *leg = new TLegend(.1, .1, .25, .3);
  leg->SetHeader("Key", "C");
  leg->AddEntry(h_pt_simu_ON_tot_pp, "PYTHIA p+#bar{p}");
  leg->AddEntry(h_pt_simu_ON_proton, "PYTHIA p");
  leg->AddEntry(h_pt_simu_ON_antiproton, "PYTHIA #bar{p}");
  leg->AddEntry(h_pt_simu_OFF_tot_pp, "PYTHIA p+#bar{p} OFF");
  leg->AddEntry(h_pt_cern_pp, "ALICE");
  leg->Draw("Same");

  return 0;
}