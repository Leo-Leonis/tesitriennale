// main142.cc is a part of the PYTHIA event generator.
// Copyright (C) 2024 Torbjorn Sjostrand.
// PYTHIA is licenced under the GNU GPL v2 or later, see COPYING for details.
// Please respect the MCnet Guidelines, see GUIDELINES for details.

// Authors: Dag Gillberg <dag.gillberg@cern.ch>

// Keywords: root; jets; event display

// This is a program to use ROOT to visualize different jet algoritms.
// The produced figure is used in the article "50 years of Quantum
// Chromodynamics" in celebration of the 50th anniversary of QCD (EPJC).

#include "Pythia8/Pythia.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"    //
#include "TLatex.h"  //
#include "TMarker.h" //
#include "TMath.h"
#include "TPave.h" //
#include "TRandom3.h"
#include "TString.h"
#include "TStyle.h"

void showProgressBar(int progress, int total) {
  int barWidth = 50; // Larghezza della barra di progresso
  float progressPercentage = (float)progress / total;

  std::cout << "[";
  int pos = barWidth * progressPercentage;

  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "#";
    else
      std::cout << " ";
  }

  std::cout
      << "] " << int(progressPercentage * 100.0) << "% (" << progress
      << " events)"
      << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
         "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
         "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b";
  std::cout.flush(); // Assicura che la barra venga aggiornata immediatamente
}

// prova con 1e6
int main142(const int nevs = 1000, const bool Dprod = 1) {

  gStyle->SetOptStat(2200);
  // gStyle->SetOptFit(1111);

  Int_t n_bins = 51; // il numero dei bin
  // i limiti dei bins customizzati malissimo del cern
  // Double_t x_max = 20;
  // Double_t x_min = 0.3;
  Double_t bin_edges[] = {.3,  .35, .4,  .45, .5,  .55, .6,  .65, .7,  .75, .8,
                          .85, .9,  .95, 1,   1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7,
                          1.8, 1.9, 2,   2.2, 2.4, 2.6, 2.8, 3,   3.2, 3.4, 3.6,
                          3.8, 4,   4.5, 5,   5.5, 6,   6.5, 7,   8,   9,   10,
                          11,  12,  13,  14,  15,  16,  18,  20};
  TH1D *h_pt_proton =
      new TH1D("h_pt_proton",
               "Proton transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
               n_bins, bin_edges);
  TH1D *h_pt_antiproton =
      new TH1D("h_pt_antiproton",
               "Antiproton transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
               n_bins, bin_edges);
  TH1D *h_pt_tot_pp =
      new TH1D("h_pt_tot_pp",
               "p#bar{p} transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
               n_bins, bin_edges);

  TH1D *h_pt_deuteron =
      new TH1D("h_pt_deuteron",
               "Deuteron transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
               n_bins, bin_edges);
  TH1D *h_pt_antideuteron =
      new TH1D("h_pt_antideuteron",
               "Antideuteron transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
               n_bins, bin_edges);
  TH1D *h_pt_tot_DD =
      new TH1D("h_pt_tot_DD",
               "D#bar{D} transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
               n_bins, bin_edges);

  // progetto failatissimo
  /*     TFile *cern_root = new TFile("cern_data/proton_pt.root");
      // TH1D *h_pt_tot_pp_cern_bins = (TH1D *)cern_root->Get("Hist1d_y1");
      TH1D *h_pt_tot_pp_cern_bins =
        new TH1D("h_pt_tot_pp_cern_bins",
                 "p#bar{p} transverse momentum distribution;Transverse "
                 "momentum #it{p}_{t} [GeV/c];Occurance [(GeV/c)^{-1}];",
                 n_bins, 0, 10); */

  // Inizializzazione impostazioni di pythia
  Pythia8::Pythia pythia;
  pythia.readString("Beams:eCM = 13000.");
  pythia.readString("Beams:idA =  2212");
  pythia.readString("Beams:idB =  2212");
  // pythia.readString("Beams:eCM = 13600.");
  pythia.readString("Beams:frameType  = 1");

  pythia.readString("Random:setSeed = on");
  pythia.readString("Random:seed = 10");
  pythia.readString("PartonVertex:setVertex = on");
  pythia.readString("Fragmentation:setVertices = on");
  pythia.readString("Tune:pp = 4");
  pythia.readString("HardQCD:all  = off");
  pythia.readString("LowEnergyQCD:all  = off");
  pythia.readString("SoftQCD:inelastic = on");
  if (Dprod == true) {
    pythia.readString("HadronLevel:DeuteronProduction = on");
  } else {
    pythia.readString("HadronLevel:DeuteronProduction = off");
  }


  // se Pythia si rompe, uccidi il programma.
  if (!pythia.init())
    return 1;

  auto &event = pythia.event;
  
  // 90'000 eventi in 3 minuti e 51sec = 231sec.
  // velocità esecuzione => 90'000/231 = 389.61 ev. per sec,
  // quindi il tempo totale di esecuzione è circa
  int exe_time = nevs / 389.61;
  int exe_minutes = exe_time / 60;
  int exe_rem_seconds = exe_time % 60;

  std::cout << "Estimated execution time: " << exe_minutes << " min e "
            << exe_rem_seconds << " sec \n";

  for (int iEvent = 0; iEvent < nevs; ++iEvent) {

    if (!pythia.next()) // se il evento generato va a male skippa al prossimo
                        // evento
      continue;

    for (int i = 0; i < event.size(); ++i) { // per ogni particella dell'evento

      // la particella i-esima dell'evento del loop
      auto &p = event[i];

      //.isResonance() && p.status() == -62) VH.push_back(p);

      if (std::abs(p.y()) > 0.5) // se la rapidità di p è maggiore di 0.5 skippa
                                 // questa particella
        continue;

      if (!p.isFinal()) // skippa se la particella non è finale (?)
        continue;

      // transverse momentum
      const float pt = std::sqrt(p.px() * p.px() + p.py() * p.py());

      // Qui riempiamo i nostri istogrammi per protoni, antiprotoni e deutoni e
      // antideutoni
      switch (p.id()) {
      case 2212: // p+
        h_pt_proton->Fill(pt);
        break;
      case -2212: // p-
        h_pt_antiproton->Fill(pt);
        break;
      case 1000010020: // d+
        h_pt_deuteron->Fill(pt);
        break;
      case -1000010020: // d-
        h_pt_antideuteron->Fill(pt);
        break;
      }
    }

    // mostra la barra di progressione
    showProgressBar(iEvent, nevs);
    /* std::cout <<
       "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
                 "\b\b\b\b\b\b\b\b\b\b\b\b"
              << "events generated: " << iEvent << '(' << 100 * iEvent / nevs
              << "%)"; */
  }

  // copia degli istogrammi non riscalati
  /* TH1D *h_proton = new TH1D(*h_pt_proton);
  TH1D *h_antiproton = new TH1D(*h_pt_antiproton);
  TH1D *h_deuteron = new TH1D(*h_pt_deuteron);
  TH1D *h_antideuteron = new TH1D(*h_pt_antideuteron); */

  // riempimento istogrammi totali dei protoni e dei deuteroni
  h_pt_tot_pp->Add(h_pt_proton, h_pt_antiproton, 1, 1);
  h_pt_tot_DD->Add(h_pt_deuteron, h_pt_antideuteron, 1, 1);

  // riscalaggio grafici (per poter confrontare queste distribuzioni e quelle di
  // cern)
  h_pt_proton->Scale(1. / nevs, "width");
  h_pt_antiproton->Scale(1. / nevs, "width");
  h_pt_deuteron->Scale(1. / nevs, "width");
  h_pt_antideuteron->Scale(1. / nevs, "width");
  h_pt_tot_pp->Scale(1. / nevs, "width");
  h_pt_tot_DD->Scale(1. / nevs, "width");

  // salvataggio grafici riscalati
  TFile *resultfile = new TFile("main142.root", "RECREATE");
  h_pt_proton->Write();
  h_pt_antiproton->Write();
  h_pt_deuteron->Write();
  h_pt_antideuteron->Write();
  h_pt_tot_pp->Write();
  h_pt_tot_DD->Write();

  // salvataggio grafici NON riscalati
  /*   TFile *testfile = new TFile("main142_NR.root", "RECREATE");
    h_proton->Write();
    h_antiproton->Write();
    h_deuteron->Write();
    h_antideuteron->Write(); */

  // presa dati di cern
  /* TFile *root_file = new TFile("cern_data/proton_pt.root");
   TH1D *h_pt_cern_pp = (TH1D *)root_file->Get("Hist1D_y1");

  TCanvas *pp_canvas = new TCanvas("pp_c", "pp_canvas", 1440, 720);
  h_pt_tot_pp->Draw("H, E, P, SAME");
  h_pt_cern_pp->Draw("H, E, P, SAME"); */

  resultfile->Close(); // chiusura file (anche se lo fa in automatico)
  return 0;
}
