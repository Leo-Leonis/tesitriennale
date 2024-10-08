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
#include "TH2D.h"
#include "TLatex.h"
#include "TMarker.h"
#include "TMath.h"
#include "TPave.h"
#include "TRandom3.h"
#include "TString.h"
#include "TStyle.h"

// prova con 1e6
int main142(const int nevs = 1000) {

  int n_bins = 200; // il numero dei bin

  TH1D *h_pt_proton =
      new TH1D("h_pt_proton",
               "Proton transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [eV];Occurance [eV^{-1}];",
               n_bins, 0, 10);
  TH1D *h_pt_antiproton =
      new TH1D("h_pt_antiproton",
               "Antiproton transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [eV];Occurance [eV^{-1}];",
               n_bins, 0, 10);
  TH1D *h_pt_deuteron =
      new TH1D("h_pt_deuteron",
               "Deuteron transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [eV];Occurance [eV^{-1}];",
               n_bins, 0, 10);
  TH1D *h_pt_antideuteron =
      new TH1D("h_pt_antideuteron",
               "Antideuteron transverse momentum distribution;Transverse "
               "momentum #it{p}_{t} [eV];Occurance [eV^{-1}];",
               n_bins, 0, 10);

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
  pythia.readString("HadronLevel:DeuteronProduction = on"); // provare on e off

  // se Pythia si rompe, uccidi il programma.
  if (!pythia.init())
    return 1;

  auto &event = pythia.event;

  for (int iEvent = 0; iEvent < nevs; ++iEvent) {
    if (!pythia.next())
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
      std::cout
          << "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
             "\b\b\b\b\b\b\b\b\b\b\b\b";
      std::cout << i;
    }
  }

  // copia degli istogrammi non riscalati
  TH1D *h_proton = new TH1D(*h_pt_proton);
  TH1D *h_antiproton = new TH1D(*h_pt_antiproton);
  TH1D *h_deuteron = new TH1D(*h_pt_deuteron);
  TH1D *h_antideuteron = new TH1D(*h_pt_antideuteron);

  // riscalaggio grafici (per poter confrontare queste distribuzioni e quelle di
  // cern)
  h_pt_proton->Scale(1. / nevs, "width");
  h_pt_antiproton->Scale(1. / nevs, "width");
  h_pt_deuteron->Scale(1. / nevs, "width");
  h_pt_antideuteron->Scale(1. / nevs, "width");

  // salvataggio grafici riscalati
  TFile *resultfile = new TFile("main142.root", "RECREATE");
  h_pt_proton->Write();
  h_pt_antiproton->Write();
  h_pt_deuteron->Write();
  h_pt_antideuteron->Write();

  // salvataggio grafici NON riscalati
  TFile *testfile = new TFile("main142_res.root", "RECREATE");
  h_proton->Write();
  h_antiproton->Write();
  h_deuteron->Write();
  h_antideuteron->Write();

  testfile->Close();
  return 0;
}
