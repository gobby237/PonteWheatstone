// plotData_zoom_lines_clipped.C
// Macro ROOT: istogramma dai dati forniti e tabella statistica.
// Le linee verticali vengono disegnate SOLO tra i bin (bordi interni)
// e si fermano all'altezza massima delle colonne.

#include <TCanvas.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <TLatex.h>
#include <TFile.h>
#include <TMath.h>
#include <TLine.h>

#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <string>
#include <TSystem.h>

void plotDataZoom() {
  gSystem->Load("libstdc++");

  // ===== 1) Dati =====
  std::vector<Double_t> data = {
    1954.9, 1953.8, 1952.8, 1952.5, 1951.8,
    1952.0, 1951.7, 1952.4, 1951.9, 1951.7,
    1951.7, 1951.4, 1951.4, 1951.4, 1951.4
  };

  if (data.empty()) {
    std::cerr << "Nessun dato.\n";
    return;
  }

  // ===== 2) Statistiche base =====
  Double_t minv = *std::min_element(data.begin(), data.end());
  Double_t maxv = *std::max_element(data.begin(), data.end());
  Double_t mean = 0, stddev = 0;
  {
    Double_t sum = 0, sum2 = 0;
    for (auto v : data) { sum += v; sum2 += v * v; }
    mean = sum / data.size();
    Double_t var = (sum2 / data.size()) - mean * mean;
    stddev = (var > 0) ? sqrt(var) : 0.0;
  }

  // ===== 3) Binning e range "allargato" =====
  const Int_t nbins = 10;
  const Double_t pad_frac = 0.20;
  const Double_t n_std = 4.0;

  Double_t span = maxv - minv;
  if (span == 0) span = 1.0;
  Double_t pad = pad_frac * span;

  Double_t xmin_data = minv - pad;
  Double_t xmax_data = maxv + pad;
  Double_t xmin_std = mean - n_std * stddev;
  Double_t xmax_std = mean + n_std * stddev;

  Double_t xmin = std::min(xmin_data, xmin_std);
  Double_t xmax = std::max(xmax_data, xmax_std);

  if (stddev == 0.0) {
    xmin = minv - pad;
    xmax = maxv + pad;
  }

  TH1D *h = new TH1D("h", "R_{x};#Omega (ohm);Counts", nbins, xmin, xmax);
  h->SetLineColor(TColor::GetColor("#bc51a1"));
  h->SetLineWidth(3);
  h->SetFillStyle(0);

  for (auto v : data) h->Fill(v);

  Double_t entries = h->GetEntries();
  Double_t underflow = h->GetBinContent(0);
  Double_t overflow = h->GetBinContent(h->GetNbinsX() + 1);
  Double_t sigma_percent = (mean != 0.0) ? (stddev / TMath::Abs(mean) * 100.0) : 0.0;

  // ===== 4) Canvas e disegno =====
  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "c", 1000, 700);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.05);

  h->Draw("HIST");

  // aumento dell'intervallo dell'asse Y (massimo +2)
  {
    double ymax = h->GetMaximum();
    h->GetYaxis()->SetRangeUser(0, ymax + 2);
  }

  // ===== 7) Salvataggio =====
  c->SaveAs("hist_plot_zoom.png");
  TFile fout("hist_plot_zoom.root", "RECREATE");
  h->Write();
  fout.Close();

  Info("plotDataZoom", "Saved hist_plot_zoom.png and hist_plot_zoom.root (xmin=%.3f xmax=%.3f)", xmin, xmax);
}