// k.cpp
// Grafico k^-1 vs Resistenza
// Uso: root -l 'k.cpp+'

#include <TGraph.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TFile.h>
#include <vector>
#include <iostream>

void plotKinverse() {
  // ===== 1) Dati: resistenze e k =====
  std::vector<Double_t> resistance = {
    27.25, 47, 99.91, 219.68, 329.54, 389.52, 469.38, 558.38, 680.06, 747.14,
    1002.1, 1206.3, 1497.1, 1797.5, 2203
  };

  std::vector<Double_t> k = {
    0.60, 0.74, 0.87, 0.93, 0.92, 0.91, 0.89, 0.87, 0.84, 0.82,
    0.77, 0.73, 0.67, 0.62, 0.56
  };

  if (resistance.empty() || k.empty() || resistance.size() != k.size()) {
    std::cerr << "Nessun dato o dimensioni non corrispondenti.\n";
    return;
  }

  // ===== 2) I dati di k sono già invertiti =====
  std::vector<Double_t> kinverse = k;

  // ===== 3) Crea il grafico (k^-1 vs R) =====
  Int_t n = kinverse.size();
  TGraph *g = new TGraph(n);

  for (Int_t i = 0; i < n; ++i) {
    g->SetPoint(i, resistance[i], kinverse[i]);
  }

  // ===== 4) Imposta stile =====
  g->SetTitle(";R_{A,B} [#Omega];k^{-1} [#muA/#Omega]");
  g->SetMarkerColor(TColor::GetColor("#bc51a1"));
  g->SetMarkerStyle(20);
  g->SetMarkerSize(1.0);

  // ===== 5) Canvas e disegno =====
  gStyle->SetOptStat(0);
  TCanvas *c = new TCanvas("c", "k inverse vs Resistance", 900, 600);
  c->SetLeftMargin(0.12);
  c->SetRightMargin(0.05);

  g->Draw("AP");
  g->GetXaxis()->SetTitleSize(0.05);
  g->GetYaxis()->SetTitleSize(0.05);
  g->GetXaxis()->SetLabelSize(0.04);
  g->GetYaxis()->SetLabelSize(0.04);
  g->GetXaxis()->SetTickLength(0.02);
  g->GetYaxis()->SetTickLength(0.02);

  // ===== 6) Salvataggio =====
  c->SaveAs("k_inverse_plot.png");
  TFile fout("k_inverse_plot.root", "RECREATE");
  g->Write();
  fout.Close();

  Info("plotKinverse", "Saved k_inverse_plot.png and k_inverse_plot.root");
}
