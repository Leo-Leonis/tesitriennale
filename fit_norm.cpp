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

void fit_norm() {
  setStyle();
  // The values and the errors on the X and Y axis
  const int n_points = 3;
  double x_vals[n_points] = {0.9, 2.76, 7};
  double y_vals[n_points] = {3.63, 2.88, 2.58};

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

  // Define a linear function
  TF1 *f = new TF1("Linear law", "([0]*x) + [1]", 0.2, 1.2);
  // Let's make the function line nicer
  f->SetLineColor(kRed);
  f->SetLineStyle(2);
  // Fit it to the graph and draw it
  f->SetParName(0, "#mu_{s}"); // attrito
  f->SetParName(1, "mg/k");
  graph->Fit(f);

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

  double_t m = f->GetParameter(0);
  double_t q = f->GetParameter(1);
  std::cout << "pendenza = " << m << '\n'
            << "intercetta = " << q << '\n'
            << '\n';

  std::cout << "1/sigma = " << (m * 13) + q << " 1/barn" << '\n';

  // deuterone -> 1/sigma = 1.7136 1/barn
  // antideut  -> 1/sigma = 1.56872 1/barn
}
