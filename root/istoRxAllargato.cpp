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
#include <TApplication.h>

#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <string>
#include <TSystem.h>

// ===== Costanti per range personalizzato asse X =====
const Double_t USER_XMIN = 1948;
const Double_t USER_XMAX = 1958;

// ===== Costante per massimo asse Y =====
const Double_t USER_YMAX = -1;

// ===== Costante per larghezza bin =====
const Double_t BIN_WIDTH = -1.0;

// ===== Costanti per dimensioni immagine in pixel =====
const Int_t IMG_WIDTH  = 500;
const Int_t IMG_HEIGHT = 600;

void plotDataZoomAllargato() {
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
  Int_t nbins = 40;
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

  // ===== Override range personalizzato =====
  if (USER_XMIN != -1.0) xmin = USER_XMIN;
  if (USER_XMAX != -1.0) xmax = USER_XMAX;

  // ===== Calcolo nbins basato su BIN_WIDTH se specificato =====
  if (BIN_WIDTH > 0.0) {
    nbins = TMath::Ceil((xmax - xmin) / BIN_WIDTH);
    if (nbins < 1) nbins = 1;
  }

  TH1D *h = new TH1D("h", "R_{X};#Omega (ohm);Counts", nbins, xmin, xmax);
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
  TCanvas *c = new TCanvas("c", "c", IMG_WIDTH, IMG_HEIGHT);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.05);

  // Applica il range visibile sull'asse X se USER_XMIN/USER_XMAX sono definiti
  if (USER_XMIN != -1 && USER_XMAX != -1) {
    h->GetXaxis()->SetRangeUser(USER_XMIN, USER_XMAX);
  }

  // Applica il massimo personalizzato sull'asse Y se definito
  if (USER_YMAX != -1) {
    h->SetMaximum(USER_YMAX);
  }

  // Nasconde la prima etichetta degli assi X e Y
  h->GetXaxis()->ChangeLabel(1, -1, 0);
  h->GetYaxis()->ChangeLabel(1, -1, 0);

  h->Draw("HIST");

  // Aggiorna il canvas per mostrare i disegni
  c->Update();
  c->Modified();

  // ===== Salvataggio =====
  c->SaveAs("istoRndir.png");
  TFile fout("istoRndir.root", "RECREATE");
  h->Write();
  fout.Close();
  std::cout << "Salvato istoRndir.png e istoRndir.root ("
            << IMG_WIDTH << "x" << IMG_HEIGHT << " px)." << std::endl;

  // ===== 5) Gestione Interattiva =====
  if (!gROOT->IsBatch()) {
    std::cout << "Grafico generato. Chiudi la finestra della Canvas per terminare." << std::endl;
    c->WaitPrimitive();
  }
}

#ifndef __CINT__
int main(int argc, char **argv) {
  TApplication theApp("App", &argc, argv);
  plotDataZoomAllargato();
  theApp.Run();
  return 0;
}
#endif