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
#include <TLegend.h>
#include <TApplication.h>

#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include <string>
#include <TSystem.h>

// ===== Costanti per range personalizzato asse X =====
const Double_t USER_XMIN = 1920;
const Double_t USER_XMAX = 1984;

// ===== Costante per massimo asse Y =====
const Double_t USER_YMAX = -1;

// ===== Costante per larghezza bin =====
const Double_t BIN_WIDTH = 2;

// ===== Costanti per dimensioni immagine in pixel =====
const Int_t IMG_WIDTH  = 500;
const Int_t IMG_HEIGHT = 600;

// ===== Costante per linea verticale tratteggiata =====
const Double_t VLINE_X = 1952.17;

void plotDataZoom() {
  gSystem->Load("libstdc++");

  // ===== 1) Dati =====
  std::vector<Double_t> data_viola = {
    1954.9, 1953.8, 1952.8, 1952.5, 1951.8,
    1952.0, 1951.7, 1952.4, 1951.9, 1951.7,
    1951.7, 1951.4, 1951.4, 1951.4, 1951.4
  };

  std::vector<Double_t> data_giallo = {
    1953.0, 1952.3, 1953.0, 1947.7, 1950.8,
    1951.7, 1947.7, 1944.7, 1951.6, 1952.5,
    1947.1, 1940.8, 1950.1, 1948.9, 1953.0
  };

  std::vector<Double_t> data_blu = {
    1956.9, 1955.4, 1952.6, 1957.3, 1952.8,
    1952.4, 1955.7, 1960.2, 1952.1, 1960.9,
    1956.3, 1962.0, 1952.7, 1953.8, 1949.7
  };

  if (data_viola.empty() || data_giallo.empty() || data_blu.empty()) {
    std::cerr << "Nessun dato.\n";
    return;
  }

  // ===== 2) Statistiche base (sul dataset viola, per il range) =====
  Double_t minv = *std::min_element(data_viola.begin(), data_viola.end());
  Double_t maxv = *std::max_element(data_viola.begin(), data_viola.end());
  Double_t mean = 0, stddev = 0;
  {
    Double_t sum = 0, sum2 = 0;
    for (auto v : data_viola) { sum += v; sum2 += v * v; }
    mean = sum / data_viola.size();
    Double_t var = (sum2 / data_viola.size()) - mean * mean;
    stddev = (var > 0) ? sqrt(var) : 0.0;
  }

  // ===== 3) Binning e range =====
  const Double_t pad_frac = 0.20;
  const Double_t n_std = 4.0;

  Double_t span = maxv - minv;
  if (span == 0) span = 1.0;
  Double_t pad = pad_frac * span;

  Double_t xmin_data = minv - pad;
  Double_t xmax_data = maxv + pad;
  Double_t xmin_std  = mean - n_std * stddev;
  Double_t xmax_std  = mean + n_std * stddev;

  Double_t xmin = (USER_XMIN != -1) ? USER_XMIN : std::min(xmin_data, xmin_std);
  Double_t xmax = (USER_XMAX != -1) ? USER_XMAX : std::max(xmax_data, xmax_std);

  if (stddev == 0.0 && USER_XMIN == -1) {
    xmin = minv - pad;
    xmax = maxv + pad;
  }

  // Calcolo numero di bin in base a BIN_WIDTH oppure numero fisso
  Int_t nbins = 30;
  if (BIN_WIDTH > 0) {
    nbins = (Int_t)TMath::Ceil((xmax - xmin) / BIN_WIDTH);
    if (nbins < 1) nbins = 1;
  }

  // ===== Creazione dei tre istogrammi con lo stesso binning =====
  TH1D *h_viola  = new TH1D("h_viola",  "R_{x};#Omega (ohm);Counts", nbins, xmin, xmax);
  TH1D *h_giallo = new TH1D("h_giallo", "R_{x};#Omega (ohm);Counts", nbins, xmin, xmax);
  TH1D *h_blu    = new TH1D("h_blu",    "R_{x};#Omega (ohm);Counts", nbins, xmin, xmax);

  // Stile viola
  h_viola->SetLineColor(TColor::GetColor("#bc51a1"));
  h_viola->SetLineWidth(3);
  h_viola->SetFillStyle(0);

  // Stile giallo
  h_giallo->SetLineColor(TColor::GetColor("#e6b800"));
  h_giallo->SetLineWidth(3);
  h_giallo->SetFillStyle(0);

  // Stile blu
  h_blu->SetLineColor(TColor::GetColor("#1a6faf"));
  h_blu->SetLineWidth(3);
  h_blu->SetFillStyle(0);

  for (auto v : data_viola)  h_viola->Fill(v);
  for (auto v : data_giallo) h_giallo->Fill(v);
  for (auto v : data_blu)    h_blu->Fill(v);

  // ===== 4) Canvas e disegno =====
  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "c", IMG_WIDTH, IMG_HEIGHT);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.05);

  // Calcola il massimo tra i tre istogrammi per impostare il range Y
  Double_t ymax_auto = std::max({ h_viola->GetMaximum(),
                                   h_giallo->GetMaximum(),
                                   h_blu->GetMaximum() });

  // Applica range X, massimo Y e label su h_blu (primo disegnato)
  if (USER_XMIN != -1 && USER_XMAX != -1) {
    h_blu->GetXaxis()->SetRangeUser(USER_XMIN, USER_XMAX);
    h_giallo->GetXaxis()->SetRangeUser(USER_XMIN, USER_XMAX);
    h_viola->GetXaxis()->SetRangeUser(USER_XMIN, USER_XMAX);
  }

  Double_t ymax_finale = (USER_YMAX != -1) ? USER_YMAX : ymax_auto * 1.25;
  h_blu->SetMaximum(ymax_finale);
  h_giallo->SetMaximum(ymax_finale);
  h_viola->SetMaximum(ymax_finale);

  // Nasconde la prima etichetta degli assi X e Y sul primo istogramma disegnato
  h_blu->GetXaxis()->ChangeLabel(1, -1, 0);
  h_blu->GetYaxis()->ChangeLabel(1, -1, 0);

  // Disegna: prima blu e giallo (sfondo), poi viola in primo piano
  h_blu->Draw("HIST");
  h_giallo->Draw("HIST SAME");
  h_viola->Draw("HIST SAME");

  // ===== Legenda =====
  TLegend *leg = new TLegend(0.65, 0.75, 0.93, 0.92);
  leg->SetBorderSize(1);
  leg->SetFillStyle(0);
  leg->SetTextSize(0.032);
  leg->AddEntry(h_viola,  "R_{x}",             "l");
  leg->AddEntry(h_giallo, "R_{n0}^{corr,dir}", "l");
  leg->AddEntry(h_blu,    "R_{n0}^{corr,inv}", "l");
  leg->Draw();

  // ===== Linea verticale tratteggiata =====
  if (VLINE_X != -1) {
    c->Update();
    Double_t ymin = gPad->GetUymin();
    Double_t ymax = gPad->GetUymax();
    TLine *vline = new TLine(VLINE_X, ymin, VLINE_X, ymax);
    vline->SetLineColor(kGray + 1);
    vline->SetLineWidth(2);
    vline->SetLineStyle(2);
    vline->Draw("SAME");
  }

  // Aggiorna il canvas
  c->Update();
  c->Modified();

  // ===== Salvataggio =====
  c->SaveAs("istoRx.png");
  TFile fout("istoRx.root", "RECREATE");
  h_viola->Write();
  h_giallo->Write();
  h_blu->Write();
  fout.Close();
  std::cout << "Salvato istoRx.png e istoRx.root ("
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
  plotDataZoom();
  theApp.Run();
  return 0;
}
#endif