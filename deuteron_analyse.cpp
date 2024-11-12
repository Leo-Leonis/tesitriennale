#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TPad.h"
#include "TStyle.h"

#include <iostream>

// NON COMPILARE QUESTO PROGRAMMA SENZA AVER PRIMA ACCESO MOBA

int deuteron_analyse(bool do_print = 0) {

  gStyle->SetPadLeftMargin(.12);
  gStyle->SetPadRightMargin(0.08);
  gStyle->SetOptStat(0); // 10 per vedere le entries

  gStyle->SetStripDecimals(kFALSE); // stessi numeri di decimali

  // prendi i file
  TFile *simu_ON_file = new TFile("simu_data/main142_D_ON_1e7.root", "READ");
  // TFile *root_deuteron_file = new TFile("cern_data/deuteron_pt.root",
  // "READ");
  /* TFile *root_antideuteron_file =
      new TFile("cern_data/antideuteron_pt.root", "READ"); */

  // histogramma totale dei deuteroni
  TH1D *h_pt_simu_deuteron =
      (TH1D *)simu_ON_file->Get("p_D_production/h_pt_deuteron");
  h_pt_simu_deuteron->SetMarkerStyle(22);
  h_pt_simu_deuteron->SetLineColor(kBlue);
  h_pt_simu_deuteron->SetMarkerColor(kBlue);
  h_pt_simu_deuteron->SetMarkerSize(1.4);
  // h_pt_simu_deuteron->GetXaxis()->SetTitleOffset(1.35);
  // h_pt_simu_deuteron->GetYaxis()->SetTitleOffset(0.90);

  ////////////////////////////////////////////////////////////////////////////
  // CASO p_n/////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  // recupera prima gli istogrammi dal file
  TH1D *h_pt_in_p_n =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_in_p_n");

  TH1D *h_pt_out_g_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_g_D");
  TH1D *h_pt_out_pi0_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_pi0_D");
  TH1D *h_pt_out_pi0_pi0_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_pi0_pi0_D");
  TH1D *h_pt_out_piP_piM_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_piP_piM_D");

  std::vector<TH1D *> h_vector_p_n;
  h_vector_p_n.push_back(h_pt_out_g_D);
  h_vector_p_n.push_back(h_pt_out_pi0_D);
  h_vector_p_n.push_back(h_pt_out_pi0_pi0_D);
  h_vector_p_n.push_back(h_pt_out_piP_piM_D);

  for (TH1D *hist : h_vector_p_n) {
    hist->SetMarkerStyle(21); // quadrato
  }

  TCanvas *p_n_canvas =
      new TCanvas("p_n", "p_n momentum distribution", 720, 720);
  // p_n_canvas->Divide(2, 1);
  // p_n_canvas->cd(1);
  p_n_canvas->SetLogx();  // log x
  p_n_canvas->SetGridx(); // griglia x
  p_n_canvas->SetGridy(); // griglia y

  // disegna gli istogrammi a partire da quello con più entries
  h_vector_p_n[1]->SetLineColor(kRed + 2);
  h_vector_p_n[1]->SetMarkerColor(kRed + 2);
  h_vector_p_n[1]->SetTitle("Deuteron #it{p}_{t} distribution from p+n");
  h_vector_p_n[3]->SetLineColor(kRed);
  h_vector_p_n[3]->SetMarkerColor(kRed);
  h_vector_p_n[2]->SetLineColor(kOrange - 3);
  h_vector_p_n[2]->SetMarkerColor(kOrange - 3);
  h_vector_p_n[0]->SetLineColor(kOrange - 2);
  h_vector_p_n[0]->SetMarkerColor(kOrange - 2);

  // e1 barre con le stanghette perpendicolari, p con il marker
  h_vector_p_n[1]->Draw("e1,p,same");
  h_vector_p_n[3]->Draw("e1,p,same");
  h_vector_p_n[2]->Draw("e1,p,same");
  h_vector_p_n[0]->Draw("e1,p,same");

  // aggiungi la legenda, "p" per disegnare il marker, "e" per gli errori
  TLegend *p_n_legend = new TLegend(.62, .7, .92, .9);
  p_n_legend->AddEntry(h_vector_p_n[1], "p+n to #pi^{0}+D", "pe");
  p_n_legend->AddEntry(h_vector_p_n[3], "p+n to #pi^{+}+#pi^{-}+D", "pe");
  p_n_legend->AddEntry(h_vector_p_n[2], "p+n to #pi^{0}+#pi^{0}+D", "pe");
  p_n_legend->AddEntry(h_vector_p_n[0], "p+n to #gamma+D", "pe");
  p_n_legend->Draw("Same");

  //////////////////////////////////////////////////////////

  // vettori di puntatori di istogrammmi normalizzati
  std::vector<TH1D *> h_vector_norm_p_n;

  // istogrammi "normalizzati", ossia divisi dal processo totale pn
  TH1D *h_pt_norm_out_g_D = new TH1D(*h_pt_out_g_D);
  TH1D *h_pt_norm_out_pi0_D = new TH1D(*h_pt_out_pi0_D);
  TH1D *h_pt_norm_out_pi0_pi0_D = new TH1D(*h_pt_out_pi0_pi0_D);
  TH1D *h_pt_norm_out_piP_piM_D = new TH1D(*h_pt_out_piP_piM_D);

  h_vector_norm_p_n.push_back(h_pt_norm_out_g_D);
  h_vector_norm_p_n.push_back(h_pt_norm_out_pi0_D);
  h_vector_norm_p_n.push_back(h_pt_norm_out_pi0_pi0_D);
  h_vector_norm_p_n.push_back(h_pt_norm_out_piP_piM_D);

  for (TH1D *hist : h_vector_norm_p_n) {
    hist->Divide(h_pt_in_p_n);
  }

  TCanvas *p_n_stack_canvas =
      new TCanvas("p_n_stacked", "stacked p_n momentum distribution", 720, 720);
  p_n_stack_canvas->cd();
  p_n_stack_canvas->SetLogx();  // log x
  p_n_stack_canvas->SetGridx(); // griglia x
  p_n_stack_canvas->SetGridy(); // griglia y

  // p_n_canvas->cd(2);

  // disegna gli istogrammi impilati, a partire da quello con più entries, e poi
  // sommati via via a quelli più piccoli.
  TH1D *h_p_n_1 = new TH1D(*h_vector_norm_p_n[1]);
  h_p_n_1->SetFillColor(kRed + 2);
  h_p_n_1->SetLineColor(kBlack);
  h_p_n_1->SetTitle("Deuteron relative #it{p}_{t} distribution from p+n");
  TH1D *h_p_n_1_3 = new TH1D(*h_p_n_1);
  h_p_n_1_3->Add(h_vector_norm_p_n[3]);
  h_p_n_1_3->SetFillColor(kRed);
  TH1D *h_p_n_1_2_3 = new TH1D(*h_p_n_1_3);
  h_p_n_1_2_3->Add(h_vector_norm_p_n[2]);
  h_p_n_1_2_3->SetFillColor(kOrange - 3);
  TH1D *h_p_n_0_1_2_3 = new TH1D(*h_p_n_1_2_3);
  h_p_n_0_1_2_3->Add(h_vector_norm_p_n[0]);
  h_p_n_0_1_2_3->SetFillColor(kYellow);

  // gli istogrammi vanno disegnati in ordine da quello più piccolo (quindi sta
  // in cima) a quello più grande, "hist" per disegnare con il tipico istogramma
  h_p_n_0_1_2_3->Draw("hist");
  h_p_n_0_1_2_3->Draw("hist,same");
  h_p_n_1_2_3->Draw("hist,same");
  h_p_n_1_3->Draw("hist,same");
  h_p_n_1->Draw("hist,same");

  // ridisegna gli assi e la griglia sul canvas (tolte dall'istogramma)
  gPad->RedrawAxis();
  gPad->RedrawAxis("G");

  // aggiungi la legenda, "f" per l'opzione fill
  TLegend *p_n_stack_legend = new TLegend(.12, .1, .42, .3);
  p_n_stack_legend->AddEntry(h_p_n_0_1_2_3, "p+n to #gamma+D", "f");
  p_n_stack_legend->AddEntry(h_p_n_1_2_3, "p+n to #pi^{0}+#pi^{0}+D", "f");
  p_n_stack_legend->AddEntry(h_p_n_1_3, "p+n to #pi^{+}+#pi^{-}+D", "f");
  p_n_stack_legend->AddEntry(h_p_n_1, "p+n to #pi^{0}+D", "f");
  p_n_stack_legend->Draw("Same");

  ////////////////////////////////////////////////////////////////////////////
  // CASO p_p/////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  // per vedere cosa faccio andare nel CASO p_n

  TH1D *h_pt_in_p_p =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_in_p_p");

  TH1D *h_pt_out_piP_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_piP_D");
  TH1D *h_pt_out_piP_pi0_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_piP_pi0_D");

  std::vector<TH1D *> h_vector_p_p;
  h_vector_p_p.push_back(h_pt_out_piP_D);
  h_vector_p_p.push_back(h_pt_out_piP_pi0_D);

  for (TH1D *hist : h_vector_p_p) {
    hist->SetMarkerStyle(21);
  }

  TCanvas *p_p_canvas =
      new TCanvas("p_p", "p_p momentum distribution", 720, 720);
  p_p_canvas->SetLogx();  // log x
  p_p_canvas->SetGridx(); // griglia x
  p_p_canvas->SetGridy(); // griglia y

  // disegna gli istogrammi
  h_vector_p_p[0]->SetLineColor(kBlue);
  h_vector_p_p[0]->SetMarkerColor(kBlue);
  h_vector_p_p[0]->SetTitle("Deuteron #it{p}_{t} distribution from p+p");
  h_vector_p_p[1]->SetLineColor(kAzure + 10);
  h_vector_p_p[1]->SetMarkerColor(kAzure + 10);

  h_vector_p_p[0]->Draw("e1,p,same");
  h_vector_p_p[1]->Draw("e1,p,same");

  // aggiungi la legenda
  TLegend *p_p_legend = new TLegend(.62, .8, .92, .9);
  p_p_legend->AddEntry(h_vector_p_p[0], "p+p to #pi^{+}+D", "pe");         //
  p_p_legend->AddEntry(h_vector_p_p[1], "p+p to #pi^{+}+#pi^{0}+D", "pe"); //
  p_p_legend->Draw("Same");

  //////////////////////////////////////////////////////////////////////////

  std::vector<TH1D *> h_vector_norm_p_p;
  TH1D *h_pt_norm_out_piP_D = new TH1D(*h_pt_out_piP_D);
  TH1D *h_pt_norm_out_piP_pi0_D = new TH1D(*h_pt_out_piP_pi0_D);

  h_vector_norm_p_p.push_back(h_pt_norm_out_piP_D);
  h_vector_norm_p_p.push_back(h_pt_norm_out_piP_pi0_D);

  for (TH1D *hist : h_vector_norm_p_p) {
    hist->Divide(h_pt_in_p_p);
  }

  TCanvas *p_p_stack_canvas =
      new TCanvas("p_p_stacked", "stacked p_p momentum distribution", 720, 720);
  p_p_stack_canvas->cd();
  p_p_stack_canvas->SetLogx();  // log x
  p_p_stack_canvas->SetGridx(); // griglia x
  p_p_stack_canvas->SetGridy(); // griglia y

  // disegna gli istogrammi impilati
  TH1D *h_p_p_0 = new TH1D(*h_vector_norm_p_p[0]);
  h_p_p_0->SetFillColor(kBlue);
  h_p_p_0->SetLineColor(kBlack);
  h_p_p_0->SetTitle("Deuteron relative #it{p}_{t} distribution from p+p");
  TH1D *h_p_p_0_1 = new TH1D(*h_p_p_0);
  h_p_p_0_1->Add(h_vector_norm_p_p[1]);
  h_p_p_0_1->SetFillColor(kAzure + 10);

  h_p_p_0_1->Draw("hist,same");
  h_p_p_0_1->Draw("hist,same");
  h_p_p_0->Draw("hist,same");

  gPad->RedrawAxis();
  gPad->RedrawAxis("G");

  // aggiungi la legenda
  TLegend *p_p_stack_legend = new TLegend(.12, .1, .42, .2);
  p_p_stack_legend->AddEntry(h_p_p_0_1, "p+p to #pi^{+}+#pi^{0}+D", "f");
  p_p_stack_legend->AddEntry(h_p_p_0, "p+p to #pi^{+}+D", "f");
  p_p_stack_legend->Draw("Same");

  ////////////////////////////////////////////////////////////////////////////
  // CASO n_n/////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////

  // per vedere cosa faccio andare nel CASO p_n

  TH1D *h_pt_in_n_n =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_in_n_n");

  TH1D *h_pt_out_piM_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_piM_D");
  TH1D *h_pt_out_piM_pi0_D =
      (TH1D *)simu_ON_file->Get("deuteron/h_pt_deuteron_out_piM_pi0_D");

  std::vector<TH1D *> h_vector_n_n;
  h_vector_n_n.push_back(h_pt_out_piM_D);
  h_vector_n_n.push_back(h_pt_out_piM_pi0_D);

  for (TH1D *hist : h_vector_n_n) {
    hist->SetMarkerStyle(21);
  }

  TCanvas *n_n_canvas =
      new TCanvas("n_n", "n_n momentum distribution", 720, 720);
  n_n_canvas->SetLogx();  // log x
  n_n_canvas->SetGridx(); // griglia x
  n_n_canvas->SetGridy(); // griglia y

  // disegna gli istogrammi
  h_vector_n_n[0]->SetLineColor(kGreen + 3);
  h_vector_n_n[0]->SetMarkerColor(kGreen + 3);
  h_vector_n_n[0]->SetTitle("Deuteron #it{p}_{t} distribution from n+n");
  h_vector_n_n[1]->SetLineColor(kSpring - 8);
  h_vector_n_n[1]->SetMarkerColor(kSpring - 8);

  h_vector_n_n[0]->Draw("e1,p,same");
  h_vector_n_n[1]->Draw("e1,p,same");

  // aggiungi la legenda
  TLegend *n_n_legend = new TLegend(.62, .8, .92, .9);
  n_n_legend->AddEntry(h_vector_n_n[0], "n+n to #pi^{-}+D", "pe");         //
  n_n_legend->AddEntry(h_vector_n_n[1], "n+n to #pi^{-}+#pi^{0}+D", "pe"); //
  n_n_legend->Draw("Same");

  //////////////////////////////////////////////////////////////////////////

  std::vector<TH1D *> h_vector_norm_n_n;
  TH1D *h_pt_norm_out_piM_D = new TH1D(*h_pt_out_piM_D);
  TH1D *h_pt_norm_out_piM_pi0_D = new TH1D(*h_pt_out_piM_pi0_D);

  h_vector_norm_n_n.push_back(h_pt_norm_out_piM_D);
  h_vector_norm_n_n.push_back(h_pt_norm_out_piM_pi0_D);

  for (TH1D *hist : h_vector_norm_n_n) {
    hist->Divide(h_pt_in_n_n);
  }

  TCanvas *n_n_stack_canvas =
      new TCanvas("n_n_stacked", "stacked n_n momentum distribution", 720, 720);
  n_n_stack_canvas->cd();
  n_n_stack_canvas->SetLogx();  // log x
  n_n_stack_canvas->SetGridx(); // griglia x
  n_n_stack_canvas->SetGridy(); // griglia y

  // disegna gli istogrammi impilati
  TH1D *h_n_n_0 = new TH1D(*h_vector_norm_n_n[0]);
  h_n_n_0->SetFillColor(kGreen + 2);
  h_n_n_0->SetLineColor(kBlack);
  h_n_n_0->SetTitle("Deuteron relative #it{p}_{t} distribution from n+n");
  TH1D *h_n_n_0_1 = new TH1D(*h_n_n_0);
  h_n_n_0_1->Add(h_vector_norm_n_n[1]);
  h_n_n_0_1->SetFillColor(kGreen);

  h_n_n_0_1->Draw("hist,same");
  h_n_n_0_1->Draw("hist,same");
  h_n_n_0->Draw("hist,same");

  gPad->RedrawAxis();
  gPad->RedrawAxis("G");

  // aggiungi la legenda
  TLegend *n_n_stack_legend = new TLegend(.12, .1, .42, .2);
  n_n_stack_legend->AddEntry(h_n_n_0_1, "n+n to #pi^{-}+#pi^{0}+D", "f");
  n_n_stack_legend->AddEntry(h_n_n_0, "n+n to #pi^{-}+D", "f");
  n_n_stack_legend->Draw("Same");

  //////////////////////////////////////////////////////////////////////////
  //////////////////OVERVIEW////////////////////////////////////////////////
  //////////////////////////////////////////////////////////////////////////

  std::vector<TH1D *> h_vector_ov;
  h_vector_ov.push_back(h_pt_in_p_n);
  h_vector_ov.push_back(h_pt_in_p_p);
  h_vector_ov.push_back(h_pt_in_n_n);

  for (TH1D *hist : h_vector_ov) {
    hist->SetMarkerStyle(21);
  }

  TCanvas *ov_canvas = new TCanvas("ov", "ov momentum distribution", 720, 720);
  // ov_canvas->Divide(2, 1);
  // ov_canvas->cd(1);
  ov_canvas->SetLogx();  // log x
  ov_canvas->SetLogy();  // log y
  ov_canvas->SetGridx(); // griglia x
  ov_canvas->SetGridy(); // griglia y

  // disegna gli istogrammi
  h_vector_ov[0]->SetLineColor(kRed);
  h_vector_ov[0]->SetMarkerColor(kRed);
  h_vector_ov[0]->SetTitle("Deuteron #it{p}_{t} distribution");
  h_vector_ov[2]->SetLineColor(kGreen + 1);
  h_vector_ov[2]->SetMarkerColor(kGreen + 1);
  h_vector_ov[1]->SetLineColor(kBlue);
  h_vector_ov[1]->SetMarkerColor(kBlue);

  h_vector_ov[0]->Draw("e1,p,same");
  h_vector_ov[2]->Draw("e1,p,same");
  h_vector_ov[1]->Draw("e1,p,same");

  // aggiungi la legenda
  TLegend *ov_legend = new TLegend(.62, .75, .92, .9);
  ov_legend->AddEntry(h_vector_ov[0], "from p+n", "pe");
  ov_legend->AddEntry(h_vector_ov[2], "from n+n", "pe");
  ov_legend->AddEntry(h_vector_ov[1], "from p+p", "pe");
  ov_legend->Draw("Same");

  //////////////////////////////////////////////////////////

  std::vector<TH1D *> h_vector_norm_ov;
  TH1D *h_pt_norm_in_p_n = new TH1D(*h_pt_in_p_n);
  TH1D *h_pt_norm_in_p_p = new TH1D(*h_pt_in_p_p);
  TH1D *h_pt_norm_in_n_n = new TH1D(*h_pt_in_n_n);

  h_vector_norm_ov.push_back(h_pt_norm_in_p_n);
  h_vector_norm_ov.push_back(h_pt_norm_in_p_p);
  h_vector_norm_ov.push_back(h_pt_norm_in_n_n);

  for (TH1D *hist : h_vector_norm_ov) {
    hist->Divide(h_pt_simu_deuteron);
  }

  TCanvas *ov_stack_canvas =
      new TCanvas("ov_stacked", "stacked ov momentum distribution", 720, 720);
  ov_stack_canvas->cd();
  ov_stack_canvas->SetLogx();  // log x
  ov_stack_canvas->SetGridx(); // griglia x
  ov_stack_canvas->SetGridy(); // griglia y

  // ov_canvas->cd(2);

  // disegna gli istogrammi impilati
  TH1D *h_ov_0 = new TH1D(*h_vector_norm_ov[0]);
  h_ov_0->SetFillColor(kRed);
  h_ov_0->SetLineColor(kBlack);
  h_ov_0->SetTitle("Deuteron relative #it{p}_{t} production distribution");
  TH1D *h_ov_0_2 = new TH1D(*h_ov_0);
  h_ov_0_2->Add(h_vector_norm_ov[2]);
  h_ov_0_2->SetFillColor(kGreen);
  TH1D *h_ov_0_1_2 = new TH1D(*h_ov_0_2);
  h_ov_0_1_2->Add(h_vector_norm_ov[1]);
  h_ov_0_1_2->SetFillColor(kBlue);
  h_ov_0_1_2->GetYaxis()->SetRangeUser(0, 1.05);

  h_ov_0_1_2->Draw("hist,same");
  h_ov_0_1_2->Draw("hist,same");
  h_ov_0_2->Draw("hist,same");
  h_ov_0->Draw("hist,same");

  gPad->RedrawAxis();
  gPad->RedrawAxis("G");

  // aggiungi la legenda
  TLegend *ov_stack_legend = new TLegend(.12, .1, .42, .25);
  ov_stack_legend->AddEntry(h_ov_0_1_2, "from p+p", "f");
  ov_stack_legend->AddEntry(h_ov_0_2, "from n+n", "f");
  ov_stack_legend->AddEntry(h_ov_0, "from p+n", "f");
  ov_stack_legend->Draw("Same");

  // salvataggio file (deve essere intenzionale)
  if (do_print == 0) {
    std::cout << "\033[1;31mLEO_INFO: generated graphs won't be saved!\033[0m"
              << '\n';
  } else {
    p_n_stack_canvas->Print("deuteron_analyse/p_n_stack.pdf");
    p_p_stack_canvas->Print("deuteron_analyse/p_p_stack.pdf");
    n_n_stack_canvas->Print("deuteron_analyse/n_n_stack.pdf");
    ov_stack_canvas->Print("deuteron_analyse/ov_stack.pdf");
    p_n_canvas->Print("deuteron_analyse/p_n.pdf");
    p_p_canvas->Print("deuteron_analyse/p_p.pdf");
    n_n_canvas->Print("deuteron_analyse/n_n.pdf");
    ov_canvas->Print("deuteron_analyse/ov_log.pdf");
  }

  return 0;
}