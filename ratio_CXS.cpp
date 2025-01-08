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

int ratio_CXS(const int analyse_id = -1, bool const do_print = 0) {

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
  case 3:
    analyse_file = "simu_data/main142_E_1e8.root";
  default:
    std::cout << "LEO_ERROR: Invalid analyse_id, allowed are: 0(A), 1(B), "
                 "2(D), 4(F)"
              << '\n';
    return 0;
  }

  gStyle->SetOptStat(0); // 10 per vedere le entries

  gStyle->SetPadLeftMargin(0.12); // in percentuale
  gStyle->SetPadRightMargin(0.08);

  gStyle->SetStripDecimals(kFALSE); // stessi numeri di decimali

  // prendi i file
  TFile *simu_xs_file = new TFile(analyse_file.c_str(), "READ");
  TFile *simu_coal_file = new TFile("simu_data/main142_E_1e8.root", "READ");
  TFile *root_deuteron_file = new TFile("cern_data/deuteron_pt.root", "READ");
  TFile *root_antideuteron_file =
      new TFile("cern_data/antideuteron_pt.root", "READ");

  //////////////////////////RATIO COAL/XSECT///////////////////////////////////

  TH1D *h_pt_xs_deuteron =
      (TH1D *)simu_xs_file->Get("p_D_production/h_pt_deuteron");
  h_pt_xs_deuteron->SetMarkerStyle(21);
  h_pt_xs_deuteron->SetLineColor(kBlue);
  h_pt_xs_deuteron->SetMarkerColor(kBlue);
  // h_pt_xs_deuteron->SetMarkerSize(1.4);

  TH1D *h_pt_xs_antideuteron =
      (TH1D *)simu_xs_file->Get("p_D_production/h_pt_antideuteron");
  h_pt_xs_antideuteron->SetMarkerStyle(21);
  h_pt_xs_antideuteron->SetLineColor(kRed);
  h_pt_xs_antideuteron->SetMarkerColor(kRed);

  TH1D *h_pt_coal_deuteron =
      (TH1D *)simu_coal_file->Get("p_D_production/h_pt_deuteron");
  h_pt_coal_deuteron->SetMarkerStyle(21);
  h_pt_coal_deuteron->SetLineColor(kBlue);
  h_pt_coal_deuteron->SetMarkerColor(kBlue);
  // h_pt_coal_deuteron->SetMarkerSize(1.4);
  h_pt_coal_deuteron->GetXaxis()->SetTitleOffset(1.30);

  TH1D *h_pt_coal_antideuteron =
      (TH1D *)simu_coal_file->Get("p_D_production/h_pt_antideuteron");
  h_pt_coal_antideuteron->SetMarkerStyle(21);
  h_pt_coal_antideuteron->SetLineColor(kRed);
  h_pt_coal_antideuteron->SetMarkerColor(kRed);

  TH1D *h_ratio_deuteron_CXS = new TH1D(*h_pt_coal_deuteron);
  h_ratio_deuteron_CXS->Divide(h_pt_xs_deuteron);
  h_ratio_deuteron_CXS->SetTitle(
      "Ratio between Coalescence and Cross Section models;Transverse momentum "
      "#it{p}_{t} [GeV/c];Ratio value");

  TH1D *h_ratio_antideuteron_CXS = new TH1D(*h_pt_coal_antideuteron);
  h_ratio_antideuteron_CXS->Divide(h_pt_xs_antideuteron);

  TCanvas *ratio_CXS_canvas =
      new TCanvas("ratio_CXS_canvas", "ratio coal/Xsection", 720, 720);
  ratio_CXS_canvas->cd();
  ratio_CXS_canvas->SetLogx();  // log x
  ratio_CXS_canvas->SetGridx(); // griglia x
  ratio_CXS_canvas->SetGridy(); // griglia y

  h_ratio_deuteron_CXS->Draw("E1, P, SAME");
  h_ratio_antideuteron_CXS->Draw("E1, P, SAME");

  std::cout << "Ratio COAL/XS dueteron ";
  weightedMeanY(h_ratio_deuteron_CXS);
  std::cout << '\n';

  std::cout << "Ratio COAL/XS antidueteron ";
  weightedMeanY(h_ratio_antideuteron_CXS);
  std::cout << '\n';

  // aggiungi la legenda
  TLegend *ratio_CSX_leg = new TLegend(.12, 0.765, .42, .9);
  ratio_CSX_leg->AddEntry(h_ratio_deuteron_CXS, "COAL./X.S. D", "pe");
  ratio_CSX_leg->AddEntry(h_ratio_antideuteron_CXS, "COAL./X.S. #bar{D}", "pe");
  ratio_CSX_leg->SetTextSize(0.035);
  ratio_CSX_leg->Draw("Same");

  // ______________________________________________________________________
  // ______________________________________________________________________
  // ______________________________________________________________________
  // ______________________________________________________________________

  // salvataggio file (deve essere intenzionale)
  if (do_print == 0) {
    std::cout << "\033[1;31mLEO_INFO: generated graphs won't be saved!\033[0m"
              << '\n';
  } else {
    ratio_CXS_canvas->Print("analyse/" + analyse_case + "/ratio_CXS.pdf");
  }

  return 0;
}