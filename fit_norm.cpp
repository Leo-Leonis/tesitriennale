#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFunction.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TMarker.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"

#include <iostream>
#include <string>

// Builds a graph with errors, displays it and saves it as an image
void setStyle() {
  gStyle->SetPalette(57);
  gStyle->SetOptTitle(0);
}

double_t custom_exp(double_t *x, double_t *p) {
  return TMath::Exp(-p[0] * (x[0] - p[1])) + p[2];
}

double_t custom_1_x(double_t *x, double_t *p) {
  return p[0] / TMath::Power(x[0] - p[1], p[2]);
}

void fit_norm(int data_id = -1) {

  setStyle();
  // The values and the errors on the X and Y axis
  const int n_points = 3;
  double x_vals[n_points] = {0.9, 2.76, 7};
  double y_vals[n_points];
  if (data_id == -1) {
    std::cout << "LEO_ERROR: please specify the intended data set (\"0\" "
                 "deuteron, \"1\" antideuteron)"
              << '\n';
    return;
  } else if (data_id == 0) {
    y_vals[0] = 3.58; // deuterone
    y_vals[1] = 2.93;
    y_vals[2] = 2.63;
  } else if (data_id == 1) {
    y_vals[0] = 3.63; // antideuterone
    y_vals[1] = 2.88;
    y_vals[2] = 2.58;
  } else {
    std::cout << "LEO_ERROR: invalid fit data code (allowed: 0,1)" << '\n';
    return;
  }

  // Instance of the graph
  TGraph *graph = new TGraph(n_points, x_vals, y_vals);
  graph->SetTitle(
      "Coefficiente di attrito legno;tan#Phi; #Deltax/cos#Phi (cm)");
  graph->GetYaxis()->SetRangeUser(1, 16); // range asse y
  // Cosmetics
  graph->SetMarkerStyle(kFullSquare);
  graph->SetMarkerColor(kAzure + 2);
  graph->SetLineWidth(0);

  // The canvas on which we'll draw the graph
  TCanvas *mycanvas = new TCanvas("mycanvas", "", 1080, 720);
  mycanvas->SetGridx();
  mycanvas->SetGridy();
  mycanvas->DrawFrame(0, 1, 14, 4, ";#sqrt{s} [TeV];1/#sigma_{0} [barn^{-1}]");

  TF1 *f1;
  TF1 *f2;

  f1 = new TF1("Linear law", "([0]*x) + [1]", 0, 14);
  f1->SetLineColor(kGreen);
  f1->SetMarkerColor(kGreen);
  f1->SetParName(0, "m");
  f1->SetParName(1, "q");
  graph->Fit(f1);

  f2 = new TF1("1/x law", custom_1_x, 0, 14, 3);
  f2->SetLineColor(kRed);
  f2->SetMarkerColor(kRed);
  f2->SetParameters(57, -15, 1);
  f2->FixParameter(1, 0);
  graph->Fit(f2, "n");

  // Draw the graph
  graph->Draw("pe,same");
  f1->Draw("l,same");
  f2->Draw("l,same");

  // Build and Draw a legend
  TLegend *leg = new TLegend(.1, .1, .3, .28);
  leg->SetFillColor(0);
  graph->SetFillColor(0);
  leg->AddEntry(graph, "ALICE");
  leg->AddEntry(f1, "linear fit");
  leg->AddEntry(f2, "1/x fit");
  leg->Draw("Same");

  double_t value_from_table_lin = -1;
  double_t value_from_table_1ox = -1;

  double_t const m = f1->GetParameter(0);
  double_t const q = f1->GetParameter(1);
  std::cout << "pendenza = " << m << '\n'
            << "intercetta = " << q << '\n'
            << '\n';

  value_from_table_lin = q + (m * 13.);

  double_t const p0 = f2->GetParameter(0);
  double_t const p1 = f2->GetParameter(1);
  double_t const p2 = f2->GetParameter(2);

  value_from_table_1ox = p0 / TMath::Power(13. - p1, p2);

  std::cout << "1/sigma = " << value_from_table_1ox << " 1/barn" << '\n';

  // vertical dashed line
  double_t const expx_v[2] = {13., 13.};
  double_t const expy_v[2] = {0., 4.};
  TGraph *dashed_v_line = new TGraph(2, expx_v, expy_v);
  dashed_v_line->SetLineStyle(7);
  dashed_v_line->SetLineWidth(2);
  dashed_v_line->SetLineColor(kGray + 1);
  dashed_v_line->Draw("l,same");

  // horizontal dashed line
  double_t const expx_h[2] = {0., 14.};
  double_t const expy_h[2] = {2.63, 2.63};
  TGraph *dashed_h_line = new TGraph(2, expx_h, expy_h);
  dashed_h_line->SetLineStyle(7);
  dashed_h_line->SetLineWidth(2);
  dashed_h_line->SetLineColor(kGray + 1);
  dashed_h_line->Draw("l,same");

  graph->Draw("pe,same");

  std::vector<TMarker *> mark_vect(4);
  mark_vect[0] = new TMarker(13, 2.63, 1);                 // default
  mark_vect[1] = new TMarker(13, value_from_table_lin, 1); // fit lin
  mark_vect[2] = new TMarker(13, value_from_table_1ox, 1); // fit 1ox
  mark_vect[3] = new TMarker(13, 2.2915107, 1);            // ottimizzato

  for (TMarker *marker : mark_vect) {
    marker->SetMarkerStyle(kFullCircle);
    marker->SetMarkerSize(1.3);
    marker->Draw("same");
  }
  mark_vect[0]->SetMarkerColor(kBlack);
  mark_vect[1]->SetMarkerColor(kGreen + 2);
  mark_vect[2]->SetMarkerColor(kRed + 2);
  mark_vect[3]->SetMarkerColor(kOrange);

  std::cout << "norm: " << 1000. / (3.179165 * value_from_table_1ox) << '\n';
}
