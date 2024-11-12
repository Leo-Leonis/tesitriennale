#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMath.h"
#include "TStyle.h"

#include <iostream>

// NON COMPILARE QUESTO PROGRAMMA SENZA AVER PRIMA ACCESO MOBA

int analyse(bool do_print = 0) {

  gStyle->SetOptStat(0); // 10 per vedere le entries

  gStyle->SetPadLeftMargin(0.12); // in percentuale
  gStyle->SetPadRightMargin(0.08);

  gStyle->SetStripDecimals(kFALSE); // stessi numeri di decimali

  // prendi i file
  // TFile *simu_ON_file = new TFile("main142.root", "READ");
  TFile *simu_ON_file = new TFile("simu_data/main142_D_ON_1e7.root", "READ");
  // TFile *simu_OFF_file = new TFile("main142.root", "READ");
  TFile *simu_OFF_file = new TFile("simu_data/main142_D_OFF_1e7.root", "READ");
  TFile *root_pp_file = new TFile("cern_data/pp_pt.root", "READ");
  TFile *root_deuteron_file = new TFile("cern_data/deuteron_pt.root", "READ");
  TFile *root_antideuteron_file =
      new TFile("cern_data/antideuteron_pt.root", "READ");

  ////////////////////////////PP DRAWING/////////////////////////////////////

  // prendi gli istogrammi
  TH1D *h_pt_simu_ON_tot_pp =
      (TH1D *)simu_ON_file->Get("p_D_production/h_pt_tot_pp");
  h_pt_simu_ON_tot_pp->SetMarkerStyle(21);
  h_pt_simu_ON_tot_pp->SetLineColor(kBlue);
  h_pt_simu_ON_tot_pp->SetMarkerColor(kBlue);
  h_pt_simu_ON_tot_pp->GetXaxis()->SetTitleOffset(1.3);
  h_pt_simu_ON_tot_pp->GetYaxis()->SetTitleOffset(1.6);

  TH1D *h_pt_simu_OFF_tot_pp =
      (TH1D *)simu_OFF_file->Get("p_D_production/h_pt_tot_pp");
  h_pt_simu_OFF_tot_pp->SetMarkerStyle(25);
  h_pt_simu_OFF_tot_pp->SetMarkerColor(kViolet);
  h_pt_simu_OFF_tot_pp->SetLineColor(kViolet);
  // h_pt_simu_OFF_tot_pp->SetMarkerSize(1.1);

  TGraphAsymmErrors *g_pt_cern_pp =
      (TGraphAsymmErrors *)root_pp_file->Get("Table 6/Graph1D_y1");
  g_pt_cern_pp->SetMarkerStyle(20);
  g_pt_cern_pp->SetLineColor(kBlack);
  // g_pt_cern_pp->SetMarkerSize(1.2);

  TCanvas *pp_canvas =
      new TCanvas("pp_pt", "pp transverse momentum distribution", 720, 720);
  // pp_canvas->cd();
  pp_canvas->SetLogx();  // log x
  pp_canvas->SetLogy();  // log y
  pp_canvas->SetGridx(); // griglia x
  pp_canvas->SetGridy(); // griglia y

  // disegna i grafici sullo stesso canvas
  h_pt_simu_ON_tot_pp->Draw("E1, P, SAME");
  g_pt_cern_pp->Draw("E1, P, SAME");
  h_pt_simu_ON_tot_pp->Draw("E1, P, SAME");
  h_pt_simu_OFF_tot_pp->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *pp_leg = new TLegend(.12, .1, .46, .3);
  pp_leg->AddEntry(h_pt_simu_ON_tot_pp, "PYTHIA p+#bar{p} ON", "pe");
  pp_leg->AddEntry(h_pt_simu_OFF_tot_pp, "PYTHIA p+#bar{p} OFF", "pe");
  pp_leg->AddEntry(g_pt_cern_pp, "ALICE p+#bar{p}", "pe");
  pp_leg->Draw("Same");

  /////////////////////////////DIFFERENCE/////////////////////////////////////

  TH1D *diff_pp_ON_OFF = new TH1D(*h_pt_simu_ON_tot_pp);
  diff_pp_ON_OFF->Add(h_pt_simu_ON_tot_pp, h_pt_simu_OFF_tot_pp, 1, -1);
  diff_pp_ON_OFF->SetMarkerColor(kBlack);
  diff_pp_ON_OFF->SetLineColor(kBlack);

  TCanvas *diff_canvas =
      new TCanvas("diff_pt", "diff pp ON and pp OFF", 720, 720);
  diff_canvas->cd();
  diff_canvas->SetLogx();  // log x
  diff_canvas->SetGridx(); // griglia x
  diff_canvas->SetGridy(); // griglia y

  // disegna i grafici sullo stesso canvas
  diff_pp_ON_OFF->Draw("E1, P, SAME");

  //////////////////////////DEUTERON DRAWING///////////////////////////////////

  TH1D *h_pt_simu_deuteron =
      (TH1D *)simu_ON_file->Get("p_D_production/h_pt_deuteron");
  h_pt_simu_deuteron->SetMarkerStyle(21);
  h_pt_simu_deuteron->SetLineColor(kBlue);
  h_pt_simu_deuteron->SetMarkerColor(kBlue);
  // h_pt_simu_deuteron->SetMarkerSize(1.4);
  h_pt_simu_deuteron->GetXaxis()->SetTitleOffset(1.30);
  h_pt_simu_deuteron->GetYaxis()->SetTitleOffset(1.60);

  TGraphAsymmErrors *g_pt_cern_deuteron =
      (TGraphAsymmErrors *)root_deuteron_file->Get(
          "Deuterons spectrum in INEL pp collisions/Graph1D_y1");
  g_pt_cern_deuteron->SetMarkerStyle(20);
  g_pt_cern_deuteron->SetLineColor(kBlack);
  // g_pt_cern_deuteron->SetMarkerSize(1.2);

  TCanvas *deuteron_canvas =
      new TCanvas("deuteron_pt", "deuteron #it{p}_{t} distribution", 720, 720);
  deuteron_canvas->cd();
  deuteron_canvas->SetLogx();  // log x
  deuteron_canvas->SetLogy();  // log y
  deuteron_canvas->SetGridx(); // griglia x
  deuteron_canvas->SetGridy(); // griglia y

  h_pt_simu_deuteron->Draw("E1, P, SAME");
  g_pt_cern_deuteron->Draw("E1, P, SAME");
  h_pt_simu_deuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *deuteron_leg = new TLegend(.12, .1, .42, .235);
  deuteron_leg->AddEntry(h_pt_simu_deuteron, "PYTHIA D", "pe");
  deuteron_leg->AddEntry(g_pt_cern_deuteron, "ALICE D", "pe");
  deuteron_leg->SetTextSize(0.035);
  deuteron_leg->Draw("Same");

  ///////////////////////////////ANTIDEUT//////////////////////////////////////

  TH1D *h_pt_simu_antideuteron =
      (TH1D *)simu_ON_file->Get("p_D_production/h_pt_antideuteron");
  h_pt_simu_antideuteron->SetMarkerStyle(21);
  h_pt_simu_antideuteron->SetLineColor(kRed);
  h_pt_simu_antideuteron->SetMarkerColor(kRed);
  h_pt_simu_antideuteron->GetXaxis()->SetTitleOffset(1.30);
  h_pt_simu_antideuteron->GetYaxis()->SetTitleOffset(1.60);

  TGraphAsymmErrors *g_pt_cern_antideuteron =
      (TGraphAsymmErrors *)root_antideuteron_file->Get(
          "Antideuterons spectrum in INEL pp collisions/Graph1D_y1");
  g_pt_cern_antideuteron->SetMarkerStyle(20);
  g_pt_cern_antideuteron->SetLineColor(kBlack);
  // g_pt_cern_antideuteron->SetMarkerSize(1.2);

  TCanvas *antideuteron_canvas = new TCanvas(
      "antideuteron_pt", "antideuteron #it{p}_{t} distribution", 720, 720);
  antideuteron_canvas->cd();
  antideuteron_canvas->SetLogx();  // log x
  antideuteron_canvas->SetLogy();  // log y
  antideuteron_canvas->SetGridx(); // griglia x
  antideuteron_canvas->SetGridy(); // griglia y

  h_pt_simu_antideuteron->Draw("E1, P, SAME");
  g_pt_cern_antideuteron->Draw("E1, P, SAME");
  h_pt_simu_antideuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *antideuteron_leg = new TLegend(.12, .1, .42, .235);
  antideuteron_leg->AddEntry(h_pt_simu_antideuteron, "PYTHIA #bar{D} ", "pe");
  antideuteron_leg->AddEntry(g_pt_cern_antideuteron, "ALICE #bar{D} ", "pe");
  antideuteron_leg->SetTextSize(0.035);
  antideuteron_leg->Draw("Same");

  ////////////////////////////RATIO
  /// DEUTERONS////////////////////////////////////

  Double_t bin_edges_cern[] = {0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.4, 1.6,
                               1.8, 2.0, 2.2, 2.6, 3.0, 3.4, 3.8};

  TH1D *h_pt_rebin_deuteron = (TH1D *)h_pt_simu_deuteron->Rebin(
      14, "h_pt_rebin_deuteron", bin_edges_cern);
  h_pt_rebin_deuteron->SetTitle("Ratio between PYTHIA and ALICE data");
  h_pt_rebin_deuteron->SetMarkerStyle(21);
  // h_pt_rebin_deuteron->SetMarkerColor(kBlue);

  TH1D *h_pt_rebin_antideuteron = (TH1D *)h_pt_simu_antideuteron->Rebin(
      14, "h_pt_rebin_antideuteron", bin_edges_cern);
  h_pt_rebin_antideuteron->SetMarkerStyle(21);
  // h_pt_rebin_antideuteron->SetMarkerColor(kRed);
  // h_pt_rebin_antideuteron->SetLineColor(kBlack);

  // conversione da TGraph a TH1D
  TH1D *h_pt_cern_deuteron =
      new TH1D("h_pt_cern_deuteron", "Deuteron #it{p}_{t} distribution", 14,
               bin_edges_cern);
  auto nPoints = g_pt_cern_deuteron->GetN(); // number of points in your TGraph
  for (int i = 0; i < nPoints; ++i) {
    double x;
    double y;
    g_pt_cern_deuteron->GetPoint(i, x, y);
    h_pt_cern_deuteron->Fill(x, y);
    h_pt_cern_deuteron->SetBinError(i, g_pt_cern_deuteron->GetErrorY(i));
  }

  TH1D *h_pt_cern_antideuteron =
      new TH1D("h_pt_cern_antideuteron", "Deuteron #it{p}_{t} distribution", 14,
               bin_edges_cern);
  for (int i = 0; i < nPoints; ++i) {
    double x;
    double y;
    g_pt_cern_antideuteron->GetPoint(i, x, y);
    h_pt_cern_antideuteron->Fill(x, y);
    h_pt_cern_antideuteron->SetBinError(i,
                                        g_pt_cern_antideuteron->GetErrorY(i));
  }

  // divisione dell'istogramma
  h_pt_rebin_deuteron->Divide(h_pt_cern_deuteron);
  h_pt_rebin_antideuteron->Divide(h_pt_cern_antideuteron);

  TCanvas *division_canvas =
      new TCanvas("division_pt", "ratio #it{p}_{t} distribution", 720, 720);
  division_canvas->cd();
  division_canvas->SetLogx();  // log x
  division_canvas->SetGridx(); // griglia x
  division_canvas->SetGridy(); // griglia y

  h_pt_rebin_deuteron->Draw("E1, P, SAME");
  h_pt_rebin_antideuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *division_leg = new TLegend(.12, .1, .42, .235);
  division_leg->AddEntry(h_pt_rebin_deuteron, "PYTHIA/ALICE D", "pe");
  division_leg->AddEntry(h_pt_rebin_antideuteron, "PYTHIA/ALICE #bar{D}", "pe");
  division_leg->SetTextSize(0.03);
  division_leg->Draw("Same");

  // calcolo del rapporto tramite media pesata

  // valore del rapporto data dalla media pesata
  double_t ratio_deuteron;
  // errore del rapporto
  double_t ratio_deuteron_err;
  // somma dei pesi (peso = 1/errore^2 = err^-2)
  double_t sum_weight_deuteron = 0;
  // numeratore di ratio_deuteron
  double_t num_deuteron = 0;
  for (int i = 0; i != nPoints; i++) {
    // peso corrente
    double_t i_weight =
        TMath::Power(h_pt_rebin_deuteron->GetBinError(i + 1), -2);
    sum_weight_deuteron += i_weight;
    num_deuteron += h_pt_rebin_deuteron->GetBinContent(i + 1) * i_weight;
  }
  ratio_deuteron = num_deuteron / sum_weight_deuteron;
  ratio_deuteron_err = TMath::Power(sum_weight_deuteron, -0.5);
  // peso del rapporto
  double_t weight_deuteron = TMath::Power(ratio_deuteron_err, -2);

  double_t ratio_antideuteron;
  double_t ratio_antideuteron_err;
  double_t sum_weight_antideuteron = 0;
  double_t num_antideuteron = 0;
  for (int i = 0; i != nPoints; i++) {
    // peso corrente
    double_t i_weight =
        TMath::Power(h_pt_rebin_antideuteron->GetBinError(i + 1), -2);
    sum_weight_antideuteron += i_weight;
    num_antideuteron +=
        h_pt_rebin_antideuteron->GetBinContent(i + 1) * i_weight;
  }
  ratio_antideuteron = num_antideuteron / sum_weight_antideuteron;
  ratio_antideuteron_err = TMath::Power(sum_weight_antideuteron, -0.5);
  double_t weight_antideuteron = TMath::Power(ratio_antideuteron_err, -2);

  std::cout << "Weighted mean PYTHIA/ALICE deuteron: " << ratio_deuteron
            << " +/- " << ratio_deuteron_err << '\n';
  std::cout << "Weighted mean PYTHIA/ALICE antideuteron: " << ratio_antideuteron
            << " +/- " << ratio_antideuteron_err << '\n';
  std::cout << "Weighted mean PYTHIA/ALICE overall: "
            << (ratio_antideuteron * weight_antideuteron +
                ratio_deuteron * weight_deuteron) /
                   (weight_antideuteron + weight_deuteron)
            << " +/- "
            << TMath::Power(weight_antideuteron + weight_deuteron, -0.5)
            << '\n';

  // ______________________________________________________________________

  // salvataggio file (deve essere intenzionale)
  if (do_print == 0) {
    std::cout << "\033[1;31mLEO_INFO: generated graphs won't be saved!\033[0m"
              << '\n';
  } else {
    pp_canvas->Print("analyse/pp.pdf");
    deuteron_canvas->Print("analyse/deuteron.pdf");
    antideuteron_canvas->Print("analyse/antideuteron.pdf");
    diff_canvas->Print("analyse/difference.pdf");
    division_canvas->Print("analyse/division.pdf");
  }

  return 0;
}