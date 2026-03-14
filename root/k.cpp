// k.cpp
// Grafico k^-1 vs Resistenza con curva teorica sovrapposta

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>
#include <TF1.h>
#include <TLegend.h>
#include <vector>
#include <iostream>
#include <cmath>

// ===== Costanti della formula =====
const Double_t E_VOLT = 10.11;    // tensione e [V]
const Double_t R_X    = 1952.17;  // resistenza Rx [Ohm]
const Double_t R_E    = 46.6;     // Re [Ohm]
const Double_t R_A    = 170.0;    // Ramp [Ohm]

Double_t kInverseFormula(Double_t *x, Double_t *) {
  Double_t R = x[0];
  Double_t num   = E_VOLT * R;
  Double_t denom = (2*R_X + R_E) * R*R
                 + R * (2*R_X*R_X + 4*R_X*R_A + 2*R_X*R_E + 2*R_E*R_A)
                 + R_X*R_X*R_E + 2*R_X*R_E*R_A;
  if (std::abs(denom) < 1e-30) return 0;
  return (num / denom) * 1e6;
}

void plotKinverse() {
  // ===== 1) Dati sperimentali =====
  std::vector<Double_t> resistance = {
    27.25, 47, 99.91, 219.68, 329.54, 389.52, 469.38, 558.38, 680.06, 747.14,
    1002.1, 1206.3, 1497.1, 1797.5, 2203
  };

  std::vector<Double_t> k = {
    0.60, 0.74, 0.87, 0.93, 0.92, 0.91, 0.89, 0.87, 0.84, 0.82,
    0.77, 0.73, 0.67, 0.62, 0.56
  };

  // ===== Incertezze su k =====
  std::vector<Double_t> k_err = {
    0.03, 0.03, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04,
    0.03, 0.03, 0.03, 0.03, 0.02
  };

  if (resistance.empty() || k.empty() || resistance.size() != k.size()
      || k_err.size() != k.size()) {
    std::cerr << "Nessun dato o dimensioni non corrispondenti.\n";
    return;
  }

  // ===== 2) Controllo valori teorici a terminale =====
  std::cout << "=== Controllo valori curva teorica ===" << std::endl;
  for (int i = 0; i < (int)resistance.size(); ++i) {
    Double_t r  = resistance[i];
    Double_t th = kInverseFormula(&r, nullptr);
    std::cout << "  Ra = " << r
              << " Ohm -> k^-1 teorico = " << th
              << "  sperimentale = " << k[i] << std::endl;
  }
  std::cout << "======================================" << std::endl;

  // ===== 3) Grafico punti sperimentali con barre di errore =====
  Int_t n = k.size();
  TGraphErrors *g = new TGraphErrors(n);
  for (Int_t i = 0; i < n; ++i) {
    g->SetPoint(i, resistance[i], k[i]);
    g->SetPointError(i, 0, k_err[i]);  // errore solo su Y, X = 0
  }

  g->SetTitle(";R_{A,B} [#Omega];k^{-1} [#muA/#Omega]");
  g->SetMarkerColor(TColor::GetColor("#bc51a1"));
  g->SetMarkerStyle(20);
  g->SetMarkerSize(1.5);
  g->SetLineColor(TColor::GetColor("#bc51a1"));  // barre errore stesso colore

  // ===== 4) Curva teorica =====
  TF1 *fteoria = new TF1("fteoria", kInverseFormula, 0, 2500, 0);
  fteoria->SetNpx(1000);
  fteoria->SetLineColor(TColor::GetColor("#1a6faf"));
  fteoria->SetLineWidth(2);

  // ===== 5) Canvas e disegno =====
  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "k inverse vs Resistance", 900, 600);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.05);

  g->SetMinimum(0.0);
  g->SetMaximum(1.1);
  g->Draw("AP");

  g->GetXaxis()->SetLimits(0, 2500);

  g->GetXaxis()->SetTitleSize(0.05);
  g->GetYaxis()->SetTitleSize(0.05);
  g->GetXaxis()->SetLabelSize(0.04);
  g->GetYaxis()->SetLabelSize(0.04);
  g->GetXaxis()->SetTickLength(0.02);
  g->GetYaxis()->SetTickLength(0.02);

  fteoria->Draw("SAME");

  // ===== 6) Legenda =====
  TLegend *leg = new TLegend(0.55, 0.20, 0.88, 0.38);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.038);
  leg->AddEntry(g,       "Dati sperimentali", "ep");
  leg->AddEntry(fteoria, "Curva teorica",      "l");
  leg->Draw();

  // ===== 7) Salvataggio =====
  c->SaveAs("k_inverse_plot.png");
  TFile fout("k_inverse_plot.root", "RECREATE");
  g->Write();
  fteoria->Write();
  fout.Close();

  Info("plotKinverse", "Saved k_inverse_plot.png and k_inverse_plot.root");
}

#ifndef __CINT__
int main(int argc, char **argv) {
  TApplication theApp("App", &argc, argv);
  plotKinverse();
  theApp.Run();
  return 0;
}
#endif