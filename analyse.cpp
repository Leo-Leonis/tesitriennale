#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TStyle.h"

// NON COMPILARE QUESTO PROGRAMMA SENZA AVER PRIMA ACCESO MOBA

int analyse() {

  gStyle->SetOptStat(0); // 10 per vedere le entries
  // gStyle->SetOptFit(1111);

  // prendi i file
  TFile *simu_ON_file = new TFile("main142_ON_1e7.root", "READ");
  TFile *simu_OFF_file = new TFile("main142_OFF_1e6.root", "READ");
  TFile *root_pp_file = new TFile("cern_data/pp_pt.root", "READ");
  TFile *root_deuteron_file = new TFile("cern_data/deuteron_pt.root", "READ");
  TFile *root_antideuteron_file =
      new TFile("cern_data/antideuteron_pt.root", "READ");
  // root_pp_file->cd();

  // prendi gli istogrammi
  TH1D *h_pt_simu_ON_tot_pp = (TH1D *)simu_ON_file->Get("h_pt_tot_pp");
  h_pt_simu_ON_tot_pp->SetMarkerStyle(22);
  h_pt_simu_ON_tot_pp->SetLineColor(kBlue);
  h_pt_simu_ON_tot_pp->SetMarkerColor(kBlue);
  h_pt_simu_ON_tot_pp->SetMarkerSize(1.4);
  h_pt_simu_ON_tot_pp->GetXaxis()->SetTitleOffset(1.35);
  h_pt_simu_ON_tot_pp->GetYaxis()->SetTitleOffset(0.90);

  // TH1D *h_pt_simu_ON_proton = (TH1D *)simu_ON_file->Get("h_pt_proton");
  // h_pt_simu_ON_proton->SetMarkerStyle(8);
  // h_pt_simu_ON_proton->SetLineColor(kRed);
  // h_pt_simu_ON_proton->SetMarkerColor(kRed);

  // TH1D *h_pt_simu_ON_antiproton = (TH1D
  // *)simu_ON_file->Get("h_pt_antiproton");
  // h_pt_simu_ON_antiproton->SetMarkerStyle(4);
  // h_pt_simu_ON_antiproton->SetLineColor(kViolet + 7);
  // h_pt_simu_ON_antiproton->SetMarkerColor(kViolet + 7);

  TH1D *h_pt_simu_OFF_tot_pp = (TH1D *)simu_OFF_file->Get("h_pt_tot_pp");
  h_pt_simu_OFF_tot_pp->SetMarkerStyle(21);
  h_pt_simu_OFF_tot_pp->SetMarkerColor(kViolet);
  h_pt_simu_OFF_tot_pp->SetLineColor(kViolet);
  h_pt_simu_OFF_tot_pp->SetMarkerSize(1.2);

  TGraphAsymmErrors *h_pt_cern_pp =
      (TGraphAsymmErrors *)root_pp_file->Get("Table 6/Graph1D_y1");
  h_pt_cern_pp->SetMarkerStyle(20);
  h_pt_cern_pp->SetLineColor(kBlack);
  h_pt_cern_pp->SetMarkerSize(1.2);

  /////////////////////////////DIFFERENCE/////////////////////////////////////

  TH1D *diff_pp_ON_OFF = new TH1D(*h_pt_simu_ON_tot_pp);
  diff_pp_ON_OFF->Add(h_pt_simu_ON_tot_pp, h_pt_simu_OFF_tot_pp, 1, -1);

  //////////////////////////DEUTERON DRAWING///////////////////////////////////

  TH1D *h_pt_simu_deuteron = (TH1D *)simu_ON_file->Get("h_pt_antideuteron");
  /*   TH1D *h_pt_simu_deuteron = (TH1D *)simu_ON_file->Get("h_pt_deuteron");
    TH1D *h_pt_simu_deuteron_in_p_n = (TH1D
    *)simu_ON_file->Get("h_pt_deuteron_in_p_n"); TH1D *h_pt_simu_deuteron_in_p_p
    = (TH1D *)simu_ON_file->Get("h_pt_deuteron_in_p_p"); TH1D
    *h_pt_simu_deuteron_in_n_n = (TH1D
    *)simu_ON_file->Get("h_pt_deuteron_in_n_n");
    h_pt_simu_deuteron->Add(h_pt_simu_deuteron_in_n_n,h_pt_simu_deuteron_in_p_p,2,1);
    h_pt_simu_deuteron->Add(h_pt_simu_deuteron_in_n_n,h_pt_simu_deuteron_in_p_n,-1,1);
   */  /* h_pt_simu_deuteron->Add(h_pt_simu_deuteron_in_n_n);
  h_pt_simu_deuteron->Add(h_pt_simu_deuteron_in_p_p); */
  h_pt_simu_deuteron->SetMarkerStyle(22);
  h_pt_simu_deuteron->SetLineColor(kBlue);
  h_pt_simu_deuteron->SetMarkerColor(kBlue);
  h_pt_simu_deuteron->SetMarkerSize(1.4);
  h_pt_simu_deuteron->GetXaxis()->SetTitleOffset(1.35);
  h_pt_simu_deuteron->GetYaxis()->SetTitleOffset(0.90);

  TGraphAsymmErrors *h_pt_cern_DD =
      (TGraphAsymmErrors *)root_deuteron_file->Get(
          "Deuterons spectrum in INEL pp collisions/Graph1D_y1");
  h_pt_cern_DD->SetMarkerStyle(20);
  h_pt_cern_DD->SetLineColor(kBlack);
  h_pt_cern_DD->SetMarkerSize(1.2);

  // drawing_________________________________________________

  TCanvas *pp_canvas =
      new TCanvas("pp_pt", "pp transverse momentum distribution", 1440, 720);
  // pp_canvas->cd();
  pp_canvas->SetLogx();  // log x
  pp_canvas->SetLogy();  // log y
  pp_canvas->SetGridx(); // griglia x
  pp_canvas->SetGridy(); // griglia y

  // disegna i grafici sullo stesso canvas
  h_pt_simu_ON_tot_pp->Draw("E, P, SAME");
  h_pt_cern_pp->Draw("E, P, SAME");
  // h_pt_simu_OFF_tot_pp->Draw("E, P, SAME");
  h_pt_simu_ON_tot_pp->Draw("E, P, SAME");
  // h_pt_simu_ON_proton->Draw("E, P, SAME");
  // h_pt_simu_ON_antiproton->Draw("E, P, SAME");

  // aggiungi la legenda
  TLegend *pp_leg = new TLegend(.1, .1, .25, .3);
  pp_leg->SetHeader("Key", "C");
  pp_leg->AddEntry(h_pt_simu_ON_tot_pp, "PYTHIA p+#bar{p}");
  // leg->AddEntry(h_pt_simu_ON_proton, "PYTHIA p");
  // leg->AddEntry(h_pt_simu_ON_antiproton, "PYTHIA #bar{p}");
  // pp_leg->AddEntry(h_pt_simu_OFF_tot_pp, "PYTHIA p+#bar{p} OFF");
  pp_leg->AddEntry(h_pt_cern_pp, "ALICE");
  pp_leg->Draw("Same");

  ////////////////////////////////////////////////////////////////

  TCanvas *diff_canvas =
      new TCanvas("diff_pt", "diff pp ON and pp OFF", 1440, 720);
  diff_canvas->cd();
  diff_canvas->SetLogx();  // log x
  // diff_canvas->SetLogy();  // log y
  diff_canvas->SetGridx(); // griglia x
  diff_canvas->SetGridy(); // griglia y

  // disegna i grafici sullo stesso canvas
  diff_pp_ON_OFF->Draw("E, P, SAME");

  // aggiungi la legenda
  /* TLegend *pp_leg = new TLegend(.1, .1, .25, .3);
  pp_leg->SetHeader("Key", "C");
  pp_leg->AddEntry(h_pt_simu_ON_tot_pp, "PYTHIA p+#bar{p}");
  // leg->AddEntry(h_pt_simu_ON_proton, "PYTHIA p");
  // leg->AddEntry(h_pt_simu_ON_antiproton, "PYTHIA #bar{p}");
  // pp_leg->AddEntry(h_pt_simu_OFF_tot_pp, "PYTHIA p+#bar{p} OFF");
  pp_leg->AddEntry(h_pt_cern_pp, "ALICE");
  pp_leg->Draw("Same"); */
  ////////////////////////////////////////////////////////////////
  TCanvas *DD_canvas =
      new TCanvas("DD_pt", "DD momentum distribution", 1440, 720);
  DD_canvas->cd();
  DD_canvas->SetLogx();  // log x
  DD_canvas->SetLogy();  // log y
  DD_canvas->SetGridx(); // griglia x
  DD_canvas->SetGridy(); // griglia y

  h_pt_simu_deuteron->Draw("E, P, SAME");
  h_pt_cern_DD->Draw("E, P, SAME");
  h_pt_simu_deuteron->Draw("E, P, SAME");

  // aggiungi la legenda
  TLegend *DD_leg = new TLegend(.1, .1, .25, .3);
  DD_leg->SetHeader("Key", "C");
  DD_leg->AddEntry(h_pt_simu_deuteron, "PYTHIA D");
  DD_leg->AddEntry(h_pt_cern_DD, "ALICE");
  DD_leg->Draw("Same");

  return 0;
}