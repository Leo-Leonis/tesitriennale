#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TMath.h"
#include "TString.h"
#include "TStyle.h"

#include <iostream>

// NON COMPILARE QUESTO PROGRAMMA SENZA AVER PRIMA ACCESO MOBA

// stampa il valore della media pesata e del suo errore
void weightedMeanY(TH1D *hist) {
  // numero di bin sull'asse x
  const int nPoints = hist->GetNbinsX();
  // somma dei pesi (peso = 1/errore^2 = err^-2)
  double_t sum_weight = 0;
  // numeratore di ratio
  double_t num = 0;

  for (int i = 0; i != nPoints; i++) {
    // contenuto del bin
    double_t const i_bin_content = hist->GetBinContent(i + 1);
    // errore del valore del bin
    double_t const i_bin_error = hist->GetBinError(i + 1);
    // peso corrente
    // std::cout << i_bin_content << " e " << i_bin_error << '\n'; // debug
    if (i_bin_content == 0. & i_bin_error == 0.) {
      // std::cout << "skipped" << '\n'; // debug
      continue;
    } else {
      double_t const i_weight = TMath::Power(i_bin_error, -2);
      sum_weight += i_weight;
      num += i_bin_content * i_weight;
      // std::cout << sum_weight << " e " << num << '\n'; // debug
    }
  }
  // valore del rapporto data dalla media pesata
  double_t const ratio = num / sum_weight;
  // errore del rapporto
  double_t const ratio_err = TMath::Power(sum_weight, -0.5);

  std::cout << "weighted mean: " << ratio << " +/- " << ratio_err;
}

int def_opt(const int analyse_id = -1, bool const do_print = 0) {

  gStyle->SetOptStat(0); // 10 per vedere le entries

  gStyle->SetPadLeftMargin(0.14); // in percentuale
  gStyle->SetPadRightMargin(0.06);

  gStyle->SetStripDecimals(kFALSE); // stessi numeri di decimali

  std::string analyse_file;
  TString analyse_case;
  switch (analyse_id) {
  case 0:
    analyse_file = "simu_data/main142_A_1e8.root";
    analyse_case = "A";
    break;
  case 1:
    analyse_file = "simu_data/main142_B_1e8.root";
    analyse_case = "B";
    break;
  case 2:
    analyse_file = "simu_data/main142_D_1e8.root";
    analyse_case = "D";
    break;
  case 4:
    analyse_file = "simu_data/main142_F_1e8.root";
    analyse_case = "F";
    break;
  case 5:
    analyse_file = "simu_data/main142_G_1e8.root";
    analyse_case = "G";
    break;
  case 3:
    analyse_file = "simu_data/main142_E_1e8.root";
  default:
    std::cout << "LEO_ERROR: Invalid analyse_id, allowed are: 0(A), 1(B), "
                 "2(D), 4(F), 5(G)"
              << '\n';
    return 0;
  }

  // prendi i file
  // TFile *simu_D_file = new TFile(analyse_file.c_str(), "READ");
  TFile *simu_D_file = new TFile("simu_data/main142_G_1e8.root", "READ");
  TFile *simu_A_file = new TFile("simu_data/main142_A_1e8.root", "READ");
  TFile *cern_deuteron_file = new TFile("cern_data/deuteron_pt.root", "READ");
  TFile *cern_antideuteron_file =
      new TFile("cern_data/antideuteron_pt.root", "READ");

  //////////////////////////RATIO DEFAULT/OPT///////////////////////////////////

  TH1D *h_pt_A_deuteron =
      (TH1D *)simu_A_file->Get("p_D_production/h_pt_deuteron");
  h_pt_A_deuteron->SetMarkerStyle(21);
  h_pt_A_deuteron->SetLineColor(kBlue);
  h_pt_A_deuteron->SetMarkerColor(kBlue);
  h_pt_A_deuteron->GetXaxis()->SetTitleOffset(1.3);
  h_pt_A_deuteron->GetXaxis()->SetRangeUser(0.65, 4);
  h_pt_A_deuteron->GetYaxis()->SetTitleOffset(1.75);
  h_pt_A_deuteron->SetTitle(
      "Deuteron #it{p}_{t} distribution;Transverse momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}} "
      "#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} [(GeV/c)^{-1}]");

  TH1D *h_pt_D_deuteron =
      (TH1D *)simu_D_file->Get("p_D_production/h_pt_deuteron");
  h_pt_D_deuteron->SetMarkerStyle(21);
  h_pt_D_deuteron->SetLineColor(kGreen + 2);
  h_pt_D_deuteron->SetMarkerColor(kGreen + 2);
  h_pt_D_deuteron->GetXaxis()->SetRangeUser(0.65, 4);

  TGraphAsymmErrors *g_pt_cern_deuteron =
      (TGraphAsymmErrors *)cern_deuteron_file->Get(
          "Deuterons spectrum in INEL pp collisions/Graph1D_y1");
  g_pt_cern_deuteron->SetMarkerStyle(20);
  g_pt_cern_deuteron->SetLineColor(kBlack);

  TCanvas *def_opt_deuteron_canvas =
      new TCanvas("def_opt_deuteron_canvas", "ratio coal/Xsection", 720, 720);
  def_opt_deuteron_canvas->cd();
  def_opt_deuteron_canvas->SetLogx();  // log x
  def_opt_deuteron_canvas->SetLogy();  // log x
  def_opt_deuteron_canvas->SetGridx(); // griglia x
  def_opt_deuteron_canvas->SetGridy(); // griglia y

  h_pt_A_deuteron->Draw("E1, P, SAME");
  g_pt_cern_deuteron->Draw("E1, P, SAME");
  h_pt_A_deuteron->Draw("E1, P, SAME");
  h_pt_D_deuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *deut_leg = new TLegend(.14, .1, .44, .235);
  deut_leg->AddEntry(h_pt_A_deuteron, "Default D", "pe");
  deut_leg->AddEntry(h_pt_D_deuteron, "Optimized D", "pe");
  deut_leg->AddEntry(g_pt_cern_deuteron, "ALICE D", "pe");
  deut_leg->SetTextSize(0.035);
  deut_leg->Draw("Same");

  ////////////////////////////////////////////////////////////////////

  TH1D *h_pt_A_antideuteron =
      (TH1D *)simu_A_file->Get("p_D_production/h_pt_antideuteron");
  h_pt_A_antideuteron->SetMarkerStyle(21);
  h_pt_A_antideuteron->SetLineColor(kRed);
  h_pt_A_antideuteron->SetMarkerColor(kRed);
  h_pt_A_antideuteron->GetXaxis()->SetTitleOffset(1.3);
  h_pt_A_antideuteron->GetXaxis()->SetRangeUser(0.65, 4);
  h_pt_A_antideuteron->GetYaxis()->SetTitleOffset(1.75);
  h_pt_A_antideuteron->SetTitle(
      "Antideuteron #it{p}_{t} distribution;Transverse momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}} "
      "#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} [(GeV/c)^{-1}]");

  TH1D *h_pt_D_antideuteron =
      (TH1D *)simu_D_file->Get("p_D_production/h_pt_antideuteron");
  h_pt_D_antideuteron->SetMarkerStyle(21);
  h_pt_D_antideuteron->SetLineColor(kViolet);
  h_pt_D_antideuteron->SetMarkerColor(kViolet);
  h_pt_D_antideuteron->GetXaxis()->SetRangeUser(0.65, 4);

  TGraphAsymmErrors *g_pt_cern_antideuteron =
      (TGraphAsymmErrors *)cern_antideuteron_file->Get(
          "Antideuterons spectrum in INEL pp collisions/Graph1D_y1");
  g_pt_cern_antideuteron->SetMarkerStyle(20);
  g_pt_cern_antideuteron->SetLineColor(kBlack);

  TCanvas *def_opt_antideuteron_canvas = new TCanvas(
      "def_opt_antideuteron_canvas", "ratio coal/Xsection", 720, 720);
  def_opt_antideuteron_canvas->cd();
  def_opt_antideuteron_canvas->SetLogx();  // log x
  def_opt_antideuteron_canvas->SetLogy();  // log x
  def_opt_antideuteron_canvas->SetGridx(); // griglia x
  def_opt_antideuteron_canvas->SetGridy(); // griglia y

  h_pt_A_antideuteron->Draw("E1, P, SAME");
  g_pt_cern_antideuteron->Draw("E1, P, SAME");
  h_pt_A_antideuteron->Draw("E1, P, SAME");
  h_pt_D_antideuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *antideut_leg = new TLegend(.14, .1, .44, .235);
  antideut_leg->AddEntry(h_pt_A_antideuteron, "Default D", "pe");
  antideut_leg->AddEntry(h_pt_D_antideuteron, "Optimized D", "pe");
  antideut_leg->AddEntry(g_pt_cern_antideuteron, "ALICE D", "pe");
  antideut_leg->SetTextSize(0.035);
  antideut_leg->Draw("Same");

  /////////////////////DEF OPT PYTHIA/ALICE RATIO/////////////////////////////

  // TGraph dati deuteroni
/*   TGraphAsymmErrors *g_pt_cern_deuteron =
      (TGraphAsymmErrors *)cern_deuteron_file->Get(
          "Deuterons spectrum in INEL pp collisions/Graph1D_y1");
  g_pt_cern_deuteron->SetMarkerStyle(20);
  g_pt_cern_deuteron->SetLineColor(kBlack);

  // TGraph dati antideuteroni
  TGraphAsymmErrors *g_pt_cern_antideuteron =
      (TGraphAsymmErrors *)cern_antideuteron_file->Get(
          "Antideuterons spectrum in INEL pp collisions/Graph1D_y1");
  g_pt_cern_antideuteron->SetMarkerStyle(20);
  g_pt_cern_antideuteron->SetLineColor(kBlack); */

  Double_t bin_edges_cern[] = {0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.4, 1.6,
                               1.8, 2.0, 2.2, 2.6, 3.0, 3.4, 3.8};

  // conversione da TGraph a TH1D
  TH1D *h_pt_cern_deuteron =
      new TH1D("h_pt_cern_deuteron", "Deuteron #it{p}_{t} distribution", 14,
               bin_edges_cern);
  // numero di punti del TGraph
  auto const nPoints = g_pt_cern_deuteron->GetN();
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

  // A deuterone rebinnato da plottare
  TH1D *h_pt_A_rebin_deuteron = (TH1D *)h_pt_A_deuteron->Rebin(
      14, "h_pt_A_rebin_deuteron", bin_edges_cern);
  h_pt_A_rebin_deuteron->SetTitle(
      "Ratio between PYTHIA and ALICE data of D;Transverse "
      "momentum #it{p}_{t} [GeV/c];Ratio value");
  h_pt_A_rebin_deuteron->SetMarkerStyle(21);
  // h_pt_rebin_deuteron->SetMarkerColor(kBlue);

  // G deuterone rebinnato da plottare
  TH1D *h_pt_D_rebin_deuteron = (TH1D *)h_pt_D_deuteron->Rebin(
      14, "h_pt_D_rebin_deuteron", bin_edges_cern);
  h_pt_D_rebin_deuteron->SetMarkerStyle(21);
  // h_pt_rebin_deuteron->SetMarkerColor(kBlue);

  // A antideut rebinnato da plottare
  TH1D *h_pt_A_rebin_antideuteron = (TH1D *)h_pt_A_antideuteron->Rebin(
      14, "h_pt_A_rebin_antideuteron", bin_edges_cern);
  h_pt_A_rebin_antideuteron->SetTitle(
      "Ratio between PYTHIA and ALICE data of #bar{D};Transverse "
      "momentum #it{p}_{t} [GeV/c];Ratio value");
  h_pt_A_rebin_antideuteron->SetMarkerStyle(21);

  // G antideut rebinnato da plottare
  TH1D *h_pt_D_rebin_antideuteron = (TH1D *)h_pt_D_antideuteron->Rebin(
      14, "h_pt_rebin_antideuteron", bin_edges_cern);
  h_pt_D_rebin_antideuteron->SetMarkerStyle(21);

  // divisione dell'istogramma
  h_pt_A_rebin_deuteron->Divide(h_pt_cern_deuteron);
  h_pt_D_rebin_deuteron->Divide(h_pt_cern_deuteron);
  h_pt_A_rebin_antideuteron->Divide(h_pt_cern_antideuteron);
  h_pt_D_rebin_antideuteron->Divide(h_pt_cern_antideuteron);

  // canvas deuteroni
  TCanvas *deuteron_division_canvas =
      new TCanvas("deuterone_division_pt", "ratio #it{p}_{t} distribution", 720, 720);
  deuteron_division_canvas->cd();
  deuteron_division_canvas->SetLogx();  // log x
  deuteron_division_canvas->SetGridx(); // griglia x
  deuteron_division_canvas->SetGridy(); // griglia y

  h_pt_A_rebin_deuteron->Draw("E1, P, SAME");
  h_pt_D_rebin_deuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *deuteron_division_leg = new TLegend(.14, .1, .6, .235);
  deuteron_division_leg->AddEntry(h_pt_A_rebin_deuteron, "PYTHIA/ALICE D default", "pe");
  deuteron_division_leg->AddEntry(h_pt_D_rebin_deuteron, "PYTHIA/ALICE D optimized", "pe");
  deuteron_division_leg->SetTextSize(0.03);
  deuteron_division_leg->Draw("Same");

  // canvas antideuteroni
  TCanvas *antideuteron_division_canvas =
      new TCanvas("deuteron_division_pt", "ratio #it{p}_{t} distribution", 720, 720);
  antideuteron_division_canvas->cd();
  antideuteron_division_canvas->SetLogx();  // log x
  antideuteron_division_canvas->SetGridx(); // griglia x
  antideuteron_division_canvas->SetGridy(); // griglia y

  h_pt_A_rebin_antideuteron->Draw("E1, P, SAME");
  h_pt_D_rebin_antideuteron->Draw("E1, P, SAME");

  // aggiungi la legenda
  TLegend *antideuteron_division_leg = new TLegend(.14, .1, .6, .235);
  antideuteron_division_leg->AddEntry(h_pt_A_rebin_antideuteron, "PYTHIA/ALICE #bar{D} default", "pe");
  antideuteron_division_leg->AddEntry(h_pt_D_rebin_antideuteron, "PYTHIA/ALICE #bar{D} optimized", "pe");
  antideuteron_division_leg->SetTextSize(0.03);
  antideuteron_division_leg->Draw("Same");

  // ______________________________________________________________________
  // ______________________________________________________________________
  // ______________________________________________________________________
  // ______________________________________________________________________

  // salvataggio file (deve essere intenzionale)
  if (do_print == 0) {
    std::cout << "\033[1;31mLEO_INFO: generated graphs won't be saved!\033[0m"
              << '\n';
  } else {
    // def_opt_deuteron_canvas->Print("analyse/G/def_opt_deuteron.pdf");
    // def_opt_antideuteron_canvas->Print("analyse/G/def_opt_antideuteron.pdf");
    deuteron_division_canvas->Print("analyse/G/def_opt_deuteron_div.pdf");
    antideuteron_division_canvas->Print("analyse/G/def_opt_antideuteron_div.pdf");
  }

  return 0;
}