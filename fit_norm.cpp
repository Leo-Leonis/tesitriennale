#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TFunction.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TLegend.h"
#include "TMath.h"
#include "TROOT.h"
#include "TStyle.h"

#include <iostream>
#include <string>

// Builds a graph with errors, displays it and saves it as an image
void setStyle() {
  gROOT->SetStyle("Plain");
  gStyle->SetOptStat(1111);
  gStyle->SetOptFit(111);
  gStyle->SetPalette(57);
  gStyle->SetOptTitle(0);
}

double_t custom_exp(double_t *x, double_t *p) {
  return TMath::Exp(-p[0] * (x[0] - p[1])) + p[2];
}

void fit_norm(int fit_id = -1, int data_id = -1) {

  if (fit_id == -1) {
    std::cout << "LEO_ERROR: please specify the intended fit (\"0\" linear, "
                 "\"1\" exponential)"
              << '\n';
    return;
  }

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
  graph->SetMarkerStyle(kOpenCircle);
  graph->SetMarkerColor(kBlue);
  graph->SetLineColor(kBlue);

  // The canvas on which we'll draw the graph
  TCanvas *mycanvas = new TCanvas();

  TF1 *f;
  // Define a linear function
  if (fit_id == 0) {
    f = new TF1("Linear law", "([0]*x) + [1]", 0.2, 14);
    f->SetLineColor(kRed);
    f->SetLineStyle(2);
    f->SetParName(0, "m");
    f->SetParName(1, "q");
    graph->Fit(f);
  } else if (fit_id == 1) {
    f = new TF1("Exponential law", custom_exp, 0.2, 14, 3);
    f->SetLineColor(kRed);
    f->SetLineStyle(2);
    graph->Fit(f);
  } else {
    std::cout << "LEO_ERROR: invalid fit id code (allowed: 0,1)" << '\n';
    return;
  }

  // Draw the graph !
  graph->Draw("APE");

  std::cout << "The Chisquare of the fit = " << graph->Chisquare(f)
            << std::endl;
  std::cout << "From function chi " << f->GetChisquare() << std::endl;
  std::cout << "From function ndf " << f->GetNDF() << std::endl;

  // Build and Draw a legend
  TLegend *leg = new TLegend(.1, .7, .3, .9);
  leg->SetFillColor(0);
  graph->SetFillColor(0);
  leg->AddEntry(graph, "Punti Sperimentali");
  leg->AddEntry(f, "Fit Lineare");
  leg->Draw("Same");

  if (fit_id == 0) {
    double_t m = f->GetParameter(0);
    double_t q = f->GetParameter(1);
    std::cout << "pendenza = " << m << '\n'
              << "intercetta = " << q << '\n'
              << '\n';

    std::cout << "1/sigma = " << (m * 13) + q << " 1/barn" << '\n';
    // deuterone -> 1/sigma = 1.7136 1/barn
    // antideut  -> 1/sigma = 1.56872 1/barn
  } else if (fit_id == 1) {
    double_t p0 = f->GetParameter(0);
    double_t p1 = f->GetParameter(1);
    double_t p2 = f->GetParameter(2);
    std::cout << "p0 = " << p0 << '\n'
              << "p1 = " << p1 << '\n'
              << "p2 = " << p2 << '\n'
              << '\n';

    std::cout << "1/sigma = " << TMath::Exp(-p0 * (13 - p1)) + p2 << " 1/barn"
              << '\n';
    // deuterone -> 1/sigma = 2.60385 1/barn
    // antideut  -> 1/sigma = 2.55994 1/barn
  } else {
    std::cout << "LEO_POETRY: ma com'è potuto succedere?" << '\n';
  }
}
