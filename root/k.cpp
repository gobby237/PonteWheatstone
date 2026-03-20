// k.cpp
// Grafico k vs Resistenza con curva teorica sovrapposta, calcolo chi quadro
// e grafico dei residui con asse X condiviso (layout a due pad)

#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>
#include <TF1.h>
#include <TLegend.h>
#include <TMath.h>
#include <TLine.h>
#include <TPad.h>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

// ===== Costanti della formula =====
const Double_t E_VOLT = 10.11;
const Double_t R_X    = 1952.17;
const Double_t R_E    = 46.6;
const Double_t R_A    = 170.0;

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
  std::vector<Double_t> k_err = {
    0.03, 0.03, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04,
    0.03, 0.03, 0.03, 0.03, 0.02
  };

  if (resistance.size() != k.size() || k_err.size() != k.size()) {
    std::cerr << "Dimensioni vettori non corrispondenti.\n";
    return;
  }

  Int_t n = k.size();

  // ===== 2) Calcolo residui, media e chi quadro =====
  std::vector<Double_t> residui(n);
  Double_t chi2      = 0.0;
  Double_t media_res = 0.0;
  Int_t    ndof      = n;

  std::cout << "=== Controllo valori curva teorica e chi quadro ===" << std::endl;
  for (int i = 0; i < n; ++i) {
    Double_t r       = resistance[i];
    Double_t k_teo   = kInverseFormula(&r, nullptr);
    residui[i]       = k[i] - k_teo;
    media_res       += residui[i];
    Double_t contrib = (residui[i] * residui[i]) / (k_err[i] * k_err[i]);
    chi2            += contrib;
    std::cout << "  Ra = "      << r
              << "  k_teo = "   << k_teo
              << "  k_exp = "   << k[i]
              << "  residuo = " << residui[i]
              << "  sigma = "   << k_err[i]
              << "  contrib = " << contrib << std::endl;
  }
  media_res /= n;

  Double_t chi2_soglia  = TMath::ChisquareQuantile(0.95, ndof);
  Double_t chi2_ridotto = chi2 / ndof;

  std::cout << "======================================" << std::endl;
  std::cout << "Chi quadro         = " << chi2          << std::endl;
  std::cout << "Gradi di liberta'  = " << ndof           << std::endl;
  std::cout << "Chi quadro ridotto = " << chi2_ridotto   << std::endl;
  std::cout << "Media dei residui  = " << media_res      << std::endl;
  std::cout << "Chi quadro soglia (95%, ndof=" << ndof << ") = " << chi2_soglia << std::endl;
  if (chi2 < chi2_soglia)
    std::cout << "COMPATIBILE: chi2 = " << chi2 << " < soglia = " << chi2_soglia << std::endl;
  else
    std::cout << "NON COMPATIBILE: chi2 = " << chi2 << " > soglia = " << chi2_soglia << std::endl;
  std::cout << "======================================" << std::endl;

  gStyle->SetOptStat(0);

  // ===== 3) Build grafi =====
  TGraphErrors *g = new TGraphErrors(n);
  for (Int_t i = 0; i < n; ++i) {
    g->SetPoint(i, resistance[i], k[i]);
    g->SetPointError(i, 0, k_err[i]);
  }
  g->SetMarkerColor(TColor::GetColor("#bc51a1"));
  g->SetMarkerStyle(20);
  g->SetMarkerSize(1.3);
  g->SetLineColor(TColor::GetColor("#bc51a1"));

  TF1 *fteoria = new TF1("fteoria", kInverseFormula, 0, 2500, 0);
  fteoria->SetNpx(1000);
  fteoria->SetLineColor(TColor::GetColor("#1a6faf"));
  fteoria->SetLineWidth(2);

  TGraphErrors *gRes = new TGraphErrors(n);
  for (Int_t i = 0; i < n; ++i) {
    gRes->SetPoint(i, resistance[i], residui[i]);
    gRes->SetPointError(i, 0, k_err[i]);
  }
  gRes->SetMarkerColor(TColor::GetColor("#bc51a1"));
  gRes->SetMarkerStyle(20);
  gRes->SetMarkerSize(1.3);
  gRes->SetLineColor(TColor::GetColor("#bc51a1"));

  // ===== 4) Canvas con due pad =====
  // Il pad superiore occupa il 70% superiore, quello inferiore il 30%
  // I margini interni vengono azzerati sul bordo di contatto così i frame
  // si toccano e l'asse X appare condiviso.
  const Double_t splitY  = 0.30;   // frazione canvas dove avviene il taglio
  const Double_t xLeft   = 0.13;
  const Double_t xRight  = 0.04;
  const Double_t xMin    = 0.0;
  const Double_t xMax    = 2500.0;

  TCanvas *c = new TCanvas("c", "k vs Resistance + Residui", 900, 750);
  c->SetFillColor(0);

  // ── Pad superiore ────────────────────────────────────────────
  TPad *pTop = new TPad("pTop", "pTop", 0.0, splitY, 1.0, 1.0);
  pTop->SetLeftMargin(xLeft);
  pTop->SetRightMargin(xRight);
  pTop->SetTopMargin(0.06);
  pTop->SetBottomMargin(0.00);   // nessun margine verso il pad inferiore
  pTop->SetFillColor(0);
  pTop->Draw();
  pTop->cd();

  g->SetTitle("");   // titolo vuoto: lo gestiamo con gli assi
  g->SetMinimum(0.0);
  g->SetMaximum(1.1);
  g->Draw("AP");
  g->GetXaxis()->SetLimits(xMin, xMax);
  g->GetYaxis()->ChangeLabel(1, -1, 0);   // nasconde lo "0" sull'asse Y

  // Nascondi completamente l'asse X del pad superiore
  g->GetXaxis()->SetLabelSize(0);
  g->GetXaxis()->SetTitleSize(0);
  g->GetXaxis()->SetTickLength(0.03);

  // Asse Y pad superiore
  g->GetYaxis()->SetTitle("k^{-1} [#muA/#Omega]");
  g->GetYaxis()->SetTitleSize(0.07);
  g->GetYaxis()->SetTitleOffset(0.80);
  g->GetYaxis()->SetLabelSize(0.06);
  g->GetYaxis()->SetTickLength(0.02);
  g->GetYaxis()->SetNdivisions(506);

  fteoria->Draw("SAME");

  TLegend *leg = new TLegend(0.55, 0.18, 0.94, 0.38);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.060);
  leg->AddEntry(g,       "Dati sperimentali", "ep");
  leg->AddEntry(fteoria, "Curva teorica",      "l");
  leg->Draw();

  // ── Pad inferiore ────────────────────────────────────────────
  c->cd();
  TPad *pBot = new TPad("pBot", "pBot", 0.0, 0.0, 1.0, splitY);
  pBot->SetLeftMargin(xLeft);
  pBot->SetRightMargin(xRight);
  pBot->SetTopMargin(0.00);      // nessun margine verso il pad superiore
  pBot->SetBottomMargin(0.35);
  pBot->SetFillColor(0);
  pBot->Draw();
  pBot->cd();

  Double_t res_max = *std::max_element(residui.begin(), residui.end());
  Double_t res_min = *std::min_element(residui.begin(), residui.end());
  Double_t err_max = *std::max_element(k_err.begin(),   k_err.end());
  Double_t margin  = err_max * 2.5;

  gRes->SetTitle("");
  gRes->SetMinimum(res_min - margin);
  gRes->SetMaximum(res_max + margin);
  gRes->Draw("AP");
  gRes->GetXaxis()->SetLimits(xMin, xMax);

  // Asse X pad inferiore (unico asse X visibile)
  gRes->GetXaxis()->SetTitle("R_{A,B} [#Omega]");
  gRes->GetXaxis()->SetTitleSize(0.14);
  gRes->GetXaxis()->SetTitleOffset(1.0);
  gRes->GetXaxis()->SetLabelSize(0.12);
  gRes->GetXaxis()->SetTickLength(0.06);
  gRes->GetXaxis()->ChangeLabel(1, -1, 0);   // nasconde il primo "0"

  // Asse Y pad inferiore
  gRes->GetYaxis()->SetTitle("Residui");
  gRes->GetYaxis()->SetTitleSize(0.12);
  gRes->GetYaxis()->SetTitleOffset(0.45);
  gRes->GetYaxis()->SetLabelSize(0.10);
  gRes->GetYaxis()->SetTickLength(0.02);
  gRes->GetYaxis()->SetNdivisions(504);

  // Linea a zero (blu tratteggiata)
  TLine *lZero = new TLine(xMin, 0, xMax, 0);
  lZero->SetLineColor(TColor::GetColor("#1a6faf"));
  lZero->SetLineWidth(2);
  lZero->SetLineStyle(2);
  lZero->Draw("SAME");




  // ===== 5) Salvataggio =====
  c->SaveAs("k_inverse_plot.png");

  TFile fout("k_inverse_plot.root", "RECREATE");
  g->Write();
  fteoria->Write();
  gRes->Write();
  fout.Close();

  Info("plotKinverse", "Salvato: k_inverse_plot.png e k_inverse_plot.root");
}

#ifndef __CINT__
int main(int argc, char **argv) {
  TApplication theApp("App", &argc, argv);
  plotKinverse();
  theApp.Run();
  return 0;
}
#endif