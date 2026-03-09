// plotData_zoom.C
// Macro ROOT: istogramma dai dati forniti e tabella statistica.
// Allarga l'asse X per visualizzare meglio la forma.
// Uso: root -l 'plotData_zoom.C+'   oppure root -l plotData_zoom.C

#include <TCanvas.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TPaveText.h>
#include <TLatex.h>
#include <TFile.h>
#include <TMath.h>

#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <string>
#include <TSystem.h>  // for gSystem->Load


void plotDataZoom() {
  // assicurati di caricare libreria standard C++ in Cling (evita errori ABI)
  gSystem->Load("libstdc++");

  // ===== 1) Dati ====
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
  Double_t mean = 0;
  Double_t stddev = 0;
  {
    Double_t sum=0, sum2=0;
    for (auto v: data) { sum += v; sum2 += v*v; }
    mean = sum / data.size();
    Double_t var = (sum2 / data.size()) - mean*mean;
    stddev = (var>0) ? sqrt(var) : 0.0;
  }

  // ===== 3) Binning e range "allargato" =====
  const Int_t nbins = 10;        // puoi cambiare
  const Double_t pad_frac = 0.20; // padding % attorno ai dati (20%)
  const Double_t n_std = 4.0;    // estendi almeno mean +/- n_std * stddev

  Double_t span = maxv - minv;
  if (span == 0) span = 1.0;
  Double_t pad = pad_frac * span;

  // range basato su dati con padding
  Double_t xmin_data = minv - pad;
  Double_t xmax_data = maxv + pad;

  // range basato su deviazione standard attorno alla media
  Double_t xmin_std = mean - n_std * stddev;
  Double_t xmax_std = mean + n_std * stddev;

  // scegli l'unione dei due range (minimo e massimo)
  Double_t xmin = std::min(xmin_data, xmin_std);
  Double_t xmax = std::max(xmax_data, xmax_std);

  // Protezione caso stddev ~ 0
  if (stddev == 0.0) {
    xmin = minv - pad;
    xmax = maxv + pad;
  }

  TH1D *h = new TH1D("h","R_{x};#Omega (ohm);Counts", nbins, xmin, xmax);
  h->SetLineColor(kRed);
  h->SetLineWidth(2);
  h->SetFillStyle(0);

  // ===== 4) Fill =====
  for (auto v : data) h->Fill(v);

  // ===== 5) Statistiche per mostrare nella tabella =====
  Double_t entries = h->GetEntries();
  Double_t underflow = h->GetBinContent(0);
  Double_t overflow = h->GetBinContent(h->GetNbinsX()+1);
  Double_t sigma_percent = (mean != 0.0) ? (stddev / TMath::Abs(mean) * 100.0) : 0.0;

  // ===== 6) Canvas e disegno =====
  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c","c",1000,700);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.30);

  h->Draw("HIST");

  // aumento dell'intervallo dell'asse Y (massimo +2)
  {
    double ymax = h->GetMaximum();
    h->GetYaxis()->SetRangeUser(0, ymax + 2);
  }

  // ===== 7) Pave con le statistiche =====
  TPaveText *pt = new TPaveText(0.72,0.45,0.98,0.9,"NDC");
  pt->SetFillColor(0);
  pt->SetLineColor(0);
  pt->SetTextAlign(12);
  pt->SetTextFont(42);
  pt->SetTextSize(0.035);

  std::ostringstream oss;
  oss << std::fixed << std::setprecision(0) << "Entries: " << entries;
  pt->AddText(oss.str().c_str()); oss.str(""); oss.clear();

  oss << std::fixed << std::setprecision(4) << "Mean: " << mean;
  pt->AddText(oss.str().c_str()); oss.str(""); oss.clear();

  oss << std::fixed << std::setprecision(4) << "Std Dev: " << stddev;
  pt->AddText(oss.str().c_str()); oss.str(""); oss.clear();

  oss << std::fixed << std::setprecision(0) << "Underflow: " << underflow;
  pt->AddText(oss.str().c_str()); oss.str(""); oss.clear();

  oss << std::fixed << std::setprecision(0) << "Overflow: " << overflow;
  pt->AddText(oss.str().c_str()); oss.str(""); oss.clear();

  oss << std::fixed << std::setprecision(2) << "#sigma / mean: " << sigma_percent << " %";
  pt->AddText(oss.str().c_str()); oss.str(""); oss.clear();

  pt->Draw();

  // ===== 9) Salvataggio opzionale =====
  c->SaveAs("hist_plot_zoom.png");
  TFile fout("hist_plot_zoom.root","RECREATE");
  h->Write();
  pt->Write("stats_pave");
  fout.Close();


  
  Info("plotDataZoom","Saved hist_plot_zoom.png and hist_plot_zoom.root  (xmin=%.3f xmax=%.3f)", xmin, xmax);
}