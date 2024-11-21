#include "Pythia8/Pythia.h"
#include "ROOT/TSeq.hxx"
#include "ROOT/TThreadedObject.hxx"
#include "TCanvas.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TH1D.h"
#include "TLatex.h"  //
#include "TMarker.h" //
#include "TMath.h"
#include "TPave.h" //
#include "TROOT.h"
#include "TRandom3.h"
#include "TStopwatch.h"
#include "TString.h"
#include "TStyle.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <thread>
#include <vector>
// #include "Pythia8Plugins/ProgressLog.h"

// ritorna la stringa del tempo da secondi al formato h,m,s
std::string formatHMSTime(double seconds) {
  int const h = static_cast<int>(seconds) / 3600;        // ore rimaste
  int const m = (static_cast<int>(seconds) % 3600) / 60; // min rimasti
  int const s = static_cast<int>(seconds) % 60;          // secondi rimasti
  std::ostringstream timeStream; // elemento ostringstream da costruire
  timeStream << std::setw(2) << std::setfill('0') << h << "h" << std::setw(2)
             << std::setfill('0') << m << "m" << std::setw(2)
             << std::setfill('0') << s << "s";
  return timeStream.str();
}

// mostra una barra di progressione
void showProgressBar(int progress, int total, double elapsed_time) {
  int const barWidth = 40; // Larghezza in pixel della barra di progresso
  float const progressPercentage = (float)progress / total;

  std::cout << "\033[33m[";

  for (int i = 0; i < barWidth; ++i) {
    if (i < barWidth * progressPercentage)
      std::cout << "#";
    else
      std::cout << " ";
  }

  // tempo totale stimato
  double const estimated_total_time = (elapsed_time / progress) * total;

  std::cout
      << "] " << int(progressPercentage * 100.0) << "% (" << progress
      << " ev., required: " << formatHMSTime(estimated_total_time)
      << ", remaining: \033[33m["
      << formatHMSTime(estimated_total_time - elapsed_time) << "\033[0m)"
      << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
         "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
         "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
         "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
  std::cout.flush(); // Assicura che la barra venga aggiornata immediatamente
}

// mostra il tempo rimanente per la generazione
void showRemainingTime(int progress, int total, double elapsed_time, int seed) {
  float const progressPercentage = (float)progress / total;

  std::cout << "\033[33m";

  // tempo totale stimato
  double const estimated_total_time = (elapsed_time / progress) * total;

  std::cout << "th. " << seed << ": " << progress << " ev., "
            << int(progressPercentage * 100.0)
            << "%, total time: " << formatHMSTime(estimated_total_time)
            << ", remaining: \033[33m"
            << formatHMSTime(estimated_total_time - elapsed_time) << "\033[0m"
            << '\n';
}

// numero di thread disponibili
const UInt_t g_poolSize = 8U;

/* void BinLogX(TH1 *h) {

  TAxis *axis = h->GetXaxis();
  int bins = axis->GetNbins();

  Axis_t from = axis->GetXmin();
  Axis_t to = axis->GetXmax();
  Axis_t width = (to - from) / bins;
  Axis_t *new_bins = new Axis_t[bins + 1];

  for (int i = 0; i <= bins; i++) {
    new_bins[i] = TMath::Power(10, from + i * width);
  }
  axis->Set(bins, new_bins);
  delete new_bins;
} */

int main142(const int nevs = 1e4, const bool Dprod = 1,
            const bool coal_mod = 0) {
  ROOT::EnableThreadSafety();

  gStyle->SetOptStat(2200);
  // gStyle->SetOptFit(1111);

  Int_t n_bins_pp = 68; // il numero dei bin per istogrammi di protoni
  Int_t n_bins_DD = 47; // il numero dei bin per deuteroni

  // il numero dei bin deve essere calcolato contando il numero dei limiti dei
  // bin sottraendo 1, quindi se ci sono 40 bin allora quello che si deve fare è
  // mettere nbin = #valori - 1

  // binnaggio pazzo del cern per istogrammi di protoni
  Double_t bin_edges_pp[] = {
      0.005, 0.01, 0.02,  0.03, 0.04,  0.05, 0.06, 0.07, 0.09, 0.11, 0.13, 0.15,
      0.175, 0.20, 0.225, 0.25, 0.275, 0.3,  0.35, 0.4,  0.45, 0.5,  0.55, 0.6,
      0.65,  0.7,  0.75,  0.8,  0.85,  0.9,  0.95, 1.0,  1.1,  1.2,  1.3,  1.4,
      1.5,   1.6,  1.7,   1.8,  1.9,   2,    2.2,  2.4,  2.6,  2.8,  3,    3.2,
      3.4,   3.6,  3.8,   4,    4.5,   5,    5.5,  6,    6.5,  7,    8,    9,
      10,    11,   12,    13,   14,    15,   16,   18,   20};
  // binnaggio pazzo del cern per istogrammi di deuteroni
  Double_t bin_edges_DD[] = {
      0.005, 0.01, 0.02,  0.03, 0.04,  0.05, 0.06, 0.07, 0.09, 0.11, 0.13, 0.15,
      0.175, 0.20, 0.225, 0.25, 0.275, 0.3,  0.35, 0.4,  0.45, 0.5,  0.55, 0.6,
      0.65,  0.7,  0.8,   0.9,  1.0,   1.1,  1.2,  1.4,  1.6,  1.8,  2,    2.2,
      2.6,   3,    3.4,   3.8,  4,     4.5,  5,    5.5,  6,    6.5,  7,    8};

  ROOT::TThreadedObject<TH1D> th_h_pt_proton(
      "h_pt_proton",
      "Proton #it{p}_{t} distribution;Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_pp, bin_edges_pp);
  ROOT::TThreadedObject<TH1D> th_h_pt_antiproton(
      "h_pt_antiproton",
      "Antiproton #it{p}_{t} distribution;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_pp, bin_edges_pp);
  ROOT::TThreadedObject<TH1D> th_h_pt_tot_pp(
      "h_pt_tot_pp",
      "p+#bar{p} #it{p}_{t} distribution;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_pp, bin_edges_pp);

  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron(
      "h_pt_deuteron",
      "Deuteron #it{p}_{t} distribution;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron(
      "h_pt_antideuteron",
      "Antideuteron #it{p}_{t} distribution;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  ROOT::TThreadedObject<TH1D> th_h_pt_tot_DD(
      "h_pt_tot_DD",
      "D+#bar{D} #it{p}_{t} distribution;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  std::vector<ROOT::TThreadedObject<TH1D> *> h_main_vector_th;
  h_main_vector_th.push_back(&th_h_pt_proton);
  h_main_vector_th.push_back(&th_h_pt_antiproton);
  h_main_vector_th.push_back(&th_h_pt_tot_pp);
  h_main_vector_th.push_back(&th_h_pt_deuteron);
  h_main_vector_th.push_back(&th_h_pt_antideuteron);
  h_main_vector_th.push_back(&th_h_pt_tot_DD);

  /////////////////////////////////////////////////////////////////////////////

  // distribuzione di D prodotti da p+n________________________________________
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_in_p_n(
      "h_pt_deuteron_in_p_n",
      "Deuteron #it{p}_{t} distribution from p+n;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  //// distribuzione di D prodotti da p+n a g+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_g_D(
      "h_pt_deuteron_out_g_D",
      "D #it{p}_{t} distribution from p+n to #gamma+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D prodotti da p+n a pi0+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_pi0_D(
      "h_pt_deuteron_out_pi0_D",
      "D #it{p}_{t} distribution from p+n to #pi^{0}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D prodotti da p+n a piP+piM+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_piP_piM_D(
      "h_pt_deuteron_out_piP_piM_D",
      "D #it{p}_{t} distribution from p+n to #pi^{+}+#pi^{-}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D prodotti da p+n a pi0+pi0+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_pi0_pi0_D(
      "h_pt_deuteron_out_pi0_pi0_D",
      "D #it{p}_{t} distribution from p+n to #pi^{0}+#pi^{0}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  std::vector<ROOT::TThreadedObject<TH1D> *> h_vector_deuteron_th;
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_in_p_n);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_g_D);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_pi0_D);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_piP_piM_D);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_pi0_pi0_D);

  // distribuzione di D prodotti da p+p________________________________________
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_in_p_p(
      "h_pt_deuteron_in_p_p",
      "Deuteron #it{p}_{t} distribution from p+p;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  //// distribuzione di D prodotti da p+p a piP+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_piP_D(
      "h_pt_deuteron_out_piP_D",
      "D #it{p}_{t} distribution from p+p to #pi^{+}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D prodotti da p+p a piP+pi0+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_piP_pi0_D(
      "h_pt_deuteron_out_piP_pi0_D",
      "D #it{p}_{t} distribution from p+p to #pi^{+}+#pi^{0}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_in_p_p);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_piP_D);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_piP_pi0_D);

  // distribuzione di D prodotti da n+n________________________________________
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_in_n_n(
      "h_pt_deuteron_in_n_n",
      "Deuteron #it{p}_{t} distribution from n+n;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  //// distribuzione di D prodotti da n+n a piM+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_piM_D(
      "h_pt_deuteron_out_piM_D",
      "D #it{p}_{t} distribution from p+p to #pi^{-}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D prodotti da n+n a piM+pi0+D
  ROOT::TThreadedObject<TH1D> th_h_pt_deuteron_out_piM_pi0_D(
      "h_pt_deuteron_out_piM_pi0_D",
      "D #it{p}_{t} distribution from p+p to #pi^{-}+#pi^{0}+D;Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_in_n_n);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_piM_D);
  h_vector_deuteron_th.push_back(&th_h_pt_deuteron_out_piM_pi0_D);

  ///////////////////////////////////////////////////////////////////////////////

  // distribuzione di D- prodotti da p+n________________________________________
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_in_p_n(
      "h_pt_antideuteron_in_p_n",
      "Antideuteron #it{p}_{t} distribution from #bar{p}+#bar{n};Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  //// distribuzione di D- prodotti da p+n a g+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_g_D(
      "h_pt_antideuteron_out_g_D",
      "#bar{D} #it{p}_{t} distribution from #bar{p}+#bar{n} to "
      "#gamma+#bar{D};Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D- prodotti da p+n a pi0+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_pi0_D(
      "h_pt_antideuteron_out_pi0_D",
      "#bar{D} #it{p}_{t} distribution from #bar{p}+#bar{n} to "
      "#pi^{0}+#bar{D};Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D- prodotti da p+n a piP+piM+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_piP_piM_D(
      "h_pt_antideuteron_out_piP_piM_D",
      "#bar{D} #it{p}_{t} distribution from #bar{p}+#bar{n} to "
      "#pi^{+}+#pi^{-}+#bar{D};Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D- prodotti da p+n a pi0+pi0+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_pi0_pi0_D(
      "h_pt_antideuteron_out_pi0_pi0_D",
      "#bar{D} #it{p}_{t} distribution from #bar{p}+#bar{n} to "
      "#pi^{0}+#pi^{0}+#bar{D};Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  std::vector<ROOT::TThreadedObject<TH1D> *> h_vector_antideuteron_th;
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_in_p_n);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_g_D);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_pi0_D);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_piP_piM_D);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_pi0_pi0_D);

  // distribuzione di D- prodotti da p+p________________________________________
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_in_p_p(
      "h_pt_antideuteron_in_p_p",
      "Antideuteron #it{p}_{t} distribution from #bar{p}+#bar{p};Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  //// distribuzione di D- prodotti da p+p a piM+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_piM_D(
      "h_pt_antideuteron_out_piM_D",
      "#bar{D} #it{p}_{t} distribution from #bar{p}+#bar{p} to "
      "#pi^{-}+#bar{D};Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D- prodotti da p+p a piM+pi0+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_pi0_piM_D(
      "h_pt_antideuteron_out_pi0_piM_D",
      "#bar{D} #it{p}_{t} distribution from #bar{p}+#bar{p} to "
      "#pi^{-}+#pi^{0}+#bar{D};Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_in_p_p);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_piM_D);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_pi0_piM_D);

  // distribuzione di D- prodotti da n+n________________________________________
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_in_n_n(
      "h_pt_antideuteron_in_n_n",
      "Antideuteron #it{p}_{t} distribution from #bar{n}+#bar{n};Transverse "
      "momentum #it{p}_{t} "
      "[GeV/c];#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  //// distribuzione di D- prodotti da n+n a piP+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_piP_D(
      "h_pt_antideuteron_out_piP_D",
      "#bar{D} #it{p}_{t} distribution from #bar{n}+#bar{n} to "
      "#pi^{+}+#bar{D};Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);
  //// distribuzione di D- prodotti da n+n a piP+pi0+D-
  ROOT::TThreadedObject<TH1D> th_h_pt_antideuteron_out_piP_pi0_D(
      "h_pt_antideuteron_out_piP_pi0_D",
      "#bar{D} #it{p}_{t} distribution from #bar{n}+#bar{n} to "
      "#pi^{+}+#pi^{0}+#bar{D};Transverse momentum #it{p}_{t} "
      "#frac{1}{#it{N}_{events}}#frac{d^{2}#it{N}}{d#it{p}_{t}d#it{y}} "
      "[(GeV/c)^{-1}];",
      n_bins_DD, bin_edges_DD);

  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_in_n_n);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_piP_D);
  h_vector_antideuteron_th.push_back(&th_h_pt_antideuteron_out_piP_pi0_D);

  // vettori di thread
  std::vector<std::thread> th_vect;

  auto main_lambda = [&](int seed) {
    // creiamo dei puntatori di istogrammi sullo stack così che accedervi costi
    // meno memoria
    std::vector<std::shared_ptr<TH1D>> h_main_vector(h_main_vector_th.size());
    for (size_t i = 0; i != h_main_vector_th.size(); i++) {
      h_main_vector[i] = h_main_vector_th[i]->Get();
    }
    std::vector<std::shared_ptr<TH1D>> h_vector_deuteron(
        h_vector_deuteron_th.size());
    for (size_t i = 0; i != h_vector_deuteron_th.size(); i++) {
      h_vector_deuteron[i] = h_vector_deuteron_th[i]->Get();
    }
    std::vector<std::shared_ptr<TH1D>> h_vector_antideuteron(
        h_vector_antideuteron_th.size());
    for (size_t i = 0; i != h_vector_antideuteron_th.size(); i++) {
      h_vector_antideuteron[i] = h_vector_antideuteron_th[i]->Get();
    }

    // Inizializzazione impostazioni di pythia
    Pythia8::Pythia pythia;
    pythia.readString("Beams:eCM = 13000.");
    pythia.readString("Beams:idA =  2212");
    pythia.readString("Beams:idB =  2212");
    pythia.readString("Beams:frameType  = 1");

    pythia.readString("Random:setSeed = on");
    pythia.readString(Form("Random:seed = 1%d", seed));
    pythia.readString("Next:numberCount = 0"); // numero dei conti disattivato
    pythia.readString("PartonVertex:setVertex = on");
    pythia.readString("Fragmentation:setVertices = on");
    pythia.readString("Tune:pp = 4");
    pythia.readString("HardQCD:all = off");
    pythia.readString("LowEnergyQCD:all = off");
    pythia.readString("SoftQCD:inelastic = on");
    if (Dprod == true) {
      // attiva la produzione di deuteroni
      pythia.readString("HadronLevel:DeuteronProduction = on");
      if (coal_mod == 1) {
        // modello di coalescenza prevede solo capture radiattiva
        // normalizzazione = 1 perché vogliamo essere certi che verrà prodotta
        // il deuterone
        pythia.readString("DeuteronProduction:norm = 1");
        // solo capture radiattiva
        pythia.readString("DeuteronProduction:channels = {2212 2112 > 22}");
        // modifica del primo canale di produzione (a quello di coalescenza)
        pythia.readString("DeuteronProduction:models = {0}");
        // modifica del param del primo canale di produzione
        pythia.readString("DeuteronProduction:parms = {0.195 1}");
      } else {

        // norm = 1000 / (3.179165 * value_from_table)
        // deuterone -> 1/sigma = 1.7136 1/barn
        // antideut  -> 1/sigma = 1.56872 1/barn
        // norm default = 119.6 [A]
        // norm deuterone = 183.5597586 [B]
        // norm antideut = 200.51252128 [C]
        // norm fit lin [A,B] = 137.26665 [D] <- abbastanza soddisfacente
        // norm fit = 1 [E] caso mod coalescenza
        // norm fit 1/x = 133.581[F]
        pythia.readString("DeuteronProduction:norm = 133.581");
      }
    } else {
      pythia.readString("HadronLevel:DeuteronProduction = off");
    }

    // se Pythia si rompe, uccidi il programma.
    if (!pythia.init()) {
      std::cout << "LEO_ERROR: Pythia initialization: negative" << '\n';
      return;
    }

    // evento corrente della simulazione
    auto &event = pythia.event;

    // array per contenere le pdg codes delle particelle prodotte da una rezione
    // con D
    std::array<int, 3> PDGarray;

    /*   // un log che tiene conto dell'andamento della simulazione
      Pythia8::ProgressLog logger(nevs); */

    TStopwatch stopwatch;
    stopwatch.Start();

    double elapsed_time;         // Tempo reale trascorso
    double estimated_total_time; // Stima del tempo totale
    double remaining_time;       // Stima del tempo rimanente

    // elapsed_time = stopwatch.RealTime();
    // std::cout << "\033[1;31mLEO_INFO: Init time: "
    //          << formatHMSTime(elapsed_time) << "\033[0m";
    // stopwatch.Reset(); // riprendi il cronometro

    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    //////////////////////////PER OGNI EVENTO///////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////
    for (int iEvent = 0; iEvent < nevs; ++iEvent) {

      if (!pythia.next()) // se il evento generato va a male skippa al prossimo
                          // evento
        continue;

      //________________________________________________________________________
      //_______________per ogni particelle dell'evento__________________________
      //________________________________________________________________________
      for (int iParticle = 0; iParticle < event.size();
           ++iParticle) { // per ogni particella dell'evento

        // la particella i-esima dell'evento del loop
        auto &p = event[iParticle];

        if (std::abs(p.y()) > 0.5) // se la rapidità di p è maggiore di 0.5
                                   // skippa questa particella
          continue;

        if (!p.isFinal()) // skippa se la particella non è finale
          continue;

        // momento trasverso della particella corrente
        const float pt = std::sqrt(p.px() * p.px() + p.py() * p.py());

        // Qui riempiamo i nostri istogrammi per protoni, antiprotoni e deutoni
        // e antideutoni
        switch (p.id()) {
        case 2212: // p+
          h_main_vector[0]->Fill(pt);
          break;
        case -2212: // p-
          h_main_vector[1]->Fill(pt);
          break;
        case 1000010020: // deuterone
        {
          PDGarray = {300, 300,
                      300}; // reset dell'array (300 perché non lo faccio
                            // corrispondere a nessuna particella)
          h_main_vector[3]->Fill(pt);

          const auto &mother1 = event[p.mother1()];
          const int mom1id = mother1.id(); // pdg della mother1
          const auto &mother2 = event[p.mother2()];
          const int mom2id = mother2.id(); // pdg della mother2

          // vettore degli indici delle particelle figlie
          const auto &daughterlist = mother1.daughterList();
          const int size = daughterlist.size();

          // fill in reactions
          if (mom1id == 2212 && mom2id == 2212) {
            h_vector_deuteron[5]->Fill(pt); // p+p
          } else if (mom1id == 2112 && mom2id == 2112) {
            h_vector_deuteron[8]->Fill(pt); // n+n
          } else if ((mom1id == 2212 && mom2id == 2112) ||
                     (mom1id == 2112 && mom2id == 2212)) {
            h_vector_deuteron[0]->Fill(pt);
          } else {
            std::cout << "\033[1;31mLEO_WARNING\033[0m: couldn't identify the "
                         "mothers! (event: "
                      << iEvent << ")" << '\n';
          }

          if (size == 2 || size == 3) {
            for (int ip = 0; ip < size; ip++) {
              PDGarray[ip] = (event[daughterlist[ip]]).id();
            }
            std::sort(PDGarray.begin(), PDGarray.end()); // riordina l'array

          } else {
            std::cout << "\033[1;31mLEO_WARNING\033[0m: Daughter list size was "
                         "neither 2 or 3 (event: "
                      << iEvent << ", PDGarray: (" << PDGarray[0] << ','
                      << PDGarray[1] << ',' << PDGarray[2] << "))\n";
            return;
          }

          if (PDGarray[2] == 1000010020) {

            // fill out reactions
            switch (PDGarray[0]) {
            // g + ... + D
            case 22:
              switch (PDGarray[1]) {
              // g + D
              case 300:
                h_vector_deuteron[1]->Fill(pt); // g + D
                break;

              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            // piP + ... + D
            case 211:
              switch (PDGarray[1]) {
              case 300:
                h_vector_deuteron[6]->Fill(pt); // piP + D
                break;

              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            // pi0 + ... + D
            case 111:
              switch (PDGarray[1]) {
              // pi0 + D
              case 300:
                h_vector_deuteron[2]->Fill(pt);
                break;
              // pi0 + pi0 + D
              case 111:
                h_vector_deuteron[4]->Fill(pt);
                break;
              // pi0 + piP + D
              case 211:
                h_vector_deuteron[7]->Fill(pt);
                break;
              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            // piM + ... + D
            case -211:
              switch (PDGarray[1]) {
              // piM + D
              case 300:
                h_vector_deuteron[9]->Fill(pt);
                break;
              // piM + piP + D
              case 211:
                h_vector_deuteron[3]->Fill(pt);
                break;
              // piM + pi0 + D
              case 111:
                h_vector_deuteron[10]->Fill(pt);
                break;

              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            default:
              std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 1 ("
                        << PDGarray[0]
                        << ") is not an expected result (PDGarray: ("
                        << PDGarray[0] << ',' << PDGarray[1] << ','
                        << PDGarray[2] << "))\n";
              break;
            }
          } else {
            std::cout << "\033[1;31m\033[1;31mLEO_WARNING\033[0m\033[0m: "
                         "daughter n. 3 is not an expected result "
                         "(expected: 1000010020, result: ("
                      << PDGarray[0] << ',' << PDGarray[1] << ',' << PDGarray[2]
                      << "))\n";
            return;
          }
        } break;          // fine deuterone
        case -1000010020: // antideuterone
        {
          h_main_vector[4]->Fill(pt);

          PDGarray = {-300, -300,
                      -300}; // reset dell'array (-300 perché non lo faccio
                             // corrispondere a nessuna particella)

          const auto &mother1 = event[p.mother1()];
          const int mom1id = mother1.id(); // pdg della mother1
          const auto &mother2 = event[p.mother2()];
          const int mom2id = mother2.id(); // pdg della mother2

          // vettore degli indici delle particelle figlie
          const auto &daughterlist = mother1.daughterList();
          const int size = daughterlist.size();

          // fill in reactions
          if (mom1id == -2212 && mom2id == -2212) {
            h_vector_antideuteron[5]->Fill(pt);
          } else if (mom1id == -2112 && mom2id == -2112) {
            h_vector_antideuteron[8]->Fill(pt);
          } else if ((mom1id == -2212 && mom2id == -2112) ||
                     (mom1id == -2112 && mom2id == -2212)) {
            h_vector_antideuteron[0]->Fill(pt);
          } else {
            std::cout << "\033[1;31mLEO_WARNING\033[0m: couldn't identify the "
                         "mothers! (event: "
                      << iEvent << ")" << '\n';
          }

          if (size == 2 || size == 3) {
            for (int ip = 0; ip < size; ip++) {
              PDGarray[ip] = (event[daughterlist[ip]]).id();
            }

            std::sort(PDGarray.begin(), PDGarray.end()); // riordina l'array

          } else {
            std::cout << "\033[1;31mLEO_WARNING\033[0m: Daughter list size was "
                         "neither 2 or 3 (event: "
                      << iEvent << ", PDGarray: (" << PDGarray[0] << ','
                      << PDGarray[1] << ',' << PDGarray[2] << "))\n";
            return;
          }

          if (PDGarray[0] == -1000010020) {

            // fill out reactions
            switch (PDGarray[2]) {
            // g + ... + D-
            case 22:
              switch (PDGarray[1]) {
              // g + D-
              case -300:
                h_vector_antideuteron[1]->Fill(pt); // g + D
                break;

              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            // piM + ... + D-
            case -211:
              switch (PDGarray[1]) {
              case -300: // piM + D-
                h_vector_antideuteron[6]->Fill(pt);
                break;

              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            // pi0 + ... + D-
            case 111:
              switch (PDGarray[1]) {
              // pi0 + D-
              case -300:
                h_vector_antideuteron[2]->Fill(pt);
                break;
              // pi0 + pi0 + D-
              case 111:
                h_vector_antideuteron[4]->Fill(pt);
                break;
              // pi0 + piM + D-
              case -211:
                h_vector_antideuteron[7]->Fill(pt);
                break;
              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            // piP + ... + D-
            case 211:
              switch (PDGarray[1]) {
              // piP + D-
              case -300:
                h_vector_antideuteron[9]->Fill(pt);
                break;
              // piP + piM + D-
              case -211:
                h_vector_antideuteron[3]->Fill(pt);
                break;
              // piP + pi0 + D-
              case 111:
                h_vector_antideuteron[10]->Fill(pt);
                break;

              default:
                std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 2 ("
                          << PDGarray[1]
                          << ") is not an expected result (PDGarray: ("
                          << PDGarray[0] << ',' << PDGarray[1] << ','
                          << PDGarray[2] << "))\n";
                break;
              }
              break;

            default:
              std::cout << "\033[1;31mLEO_WARNING\033[0m: daughter n. 3 ("
                        << PDGarray[2]
                        << ") is not an expected result (PDGarray: ("
                        << PDGarray[0] << ',' << PDGarray[1] << ','
                        << PDGarray[2] << "))\n";
              break;
            }
          } else {
            std::cout << "\033[1;31mLEO_WARNING\033[0m: "
                         "daughter n. 1 is not an expected result "
                         "(expected: -1000010020, result: ("
                      << PDGarray[0] << ',' << PDGarray[1] << ',' << PDGarray[2]
                      << "))\n";
            return;
          }
        } // fine antideuterone
        break;
        }
      }

      if (iEvent % 1000 == 0) {
        elapsed_time = stopwatch.RealTime();
        stopwatch.Continue(); // riprendi il cronometro
        // mostra la barra di progressione
        showRemainingTime(iEvent, nevs, elapsed_time, seed);
        // std::cout << "event of th " << seed << ": " << iEvent << '\n';
      }
      // logger(iEvent);
    }
    return;
  }; // fine simulazione

  // sequenza di seed che va da 0 a g_poolSize
  auto seeds = ROOT::TSeqI(0, g_poolSize);

  std::cout << "debug 1" << '\n'; // debug
  // comincia la generazione multithread
  for (auto seed : ROOT::TSeqI(seeds)) {
    th_vect.emplace_back(main_lambda, seed);
  }

  // aspetta per il termine di altri thread
  for (auto &&i_th : th_vect) {
    std::cout << "debug 1a" << '\n'; // debug
    i_th.join();
  }

  std::cout << "debug 2" << '\n'; // debug

  std::vector<std::shared_ptr<TH1D>> h_main_vector(h_main_vector_th.size());
  for (size_t i = 0; i != h_main_vector.size(); i++) {
    h_main_vector[i] = h_main_vector_th[i]->Merge();
  }
  std::cout << "debug 3.1" << '\n'; // debug

  std::vector<std::shared_ptr<TH1D>> h_vector_deuteron(
      h_vector_deuteron_th.size());
  for (size_t i = 0; i != h_vector_deuteron.size(); i++) {
    h_vector_deuteron[i] = h_vector_deuteron_th[i]->Merge();
  }
  std::cout << "debug 3.2" << '\n'; // debug

  std::vector<std::shared_ptr<TH1D>> h_vector_antideuteron(
      h_vector_antideuteron_th.size());
  for (size_t i = 0; i != h_vector_antideuteron_th.size(); i++) {
    h_vector_antideuteron[i] = h_vector_antideuteron_th[i]->Merge();
  }
  std::cout << "debug 3.3" << '\n'; // debug

  // riempimento istogrammi totali dei protoni e dei deuteroni
  /* h_pt_tot_pp->Add(h_pt_proton, h_pt_antiproton, 1, 1);
  h_pt_tot_DD->Add(h_pt_deuteron, h_pt_antideuteron, 1, 1); */

  h_main_vector[2]->Add(h_main_vector[0].get(), h_main_vector[1].get(), 1, 1);
  h_main_vector[5]->Add(h_main_vector[3].get(), h_main_vector[4].get(), 1, 1);

  // lambda da applicare a tutti gli istogrammi
  auto action_hist_lambda = [nevs](std::shared_ptr<TH1D> hist) {
    if (hist) {
      /// std::cout << "vector of hist" << '\n';
      // riscalaggio grafici
      hist->Scale(1. / (nevs * g_poolSize), "width");
      // salvataggio grafici
      hist->Write();
    }
  };

  std::cout << "debug 4" << '\n'; // debug

  // file di output
  TFile *resultfile = new TFile("main142_th.root", "RECREATE");
  // sotto directory
  TDirectory *p_D_dir = resultfile->mkdir("p_D_production");
  TDirectory *deuteron_dir = resultfile->mkdir("deuteron");
  TDirectory *antideuteron_dir = resultfile->mkdir("antideuteron");

  gDirectory->cd("p_D_production");
  // riscala e salva gli istogrammi
  std::for_each(h_main_vector.begin(), h_main_vector.end(), action_hist_lambda);
  gDirectory->cd("..");

  // vai nella cartella deuteron
  gDirectory->cd("deuteron");
  std::for_each(h_vector_deuteron.begin(), h_vector_deuteron.end(),
                action_hist_lambda);
  gDirectory->cd("..");

  // vai nella cartella antideuteron
  gDirectory->cd("antideuteron");
  std::for_each(h_vector_antideuteron.begin(), h_vector_antideuteron.end(),
                action_hist_lambda);
  gDirectory->cd("..");
  std::cout << "debug 5" << '\n'; // debug

  resultfile->Close(); // chiusura file (anche se lo fa in automatico)
  std::cout << "debug 6" << '\n'; // debug

  return 0;
}