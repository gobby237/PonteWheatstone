#include "TF1.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TApplication.h"

void gaussian() {
    // Crea l'applicazione ROOT
    TApplication *app = new TApplication("app", 0, 0);
    
    // Crea una funzione gaussiana con media 0 e deviazione standard 1
    TF1 *f1 = new TF1("gaussian", "TMath::Gaus(x, 0, 1)", -4, 4);
    
    // Imposta il titolo e le etichette degli assi
    f1->SetTitle("Gaussiana con media 0 e #sigma = 1");
    f1->GetXaxis()->SetTitle("x");
    f1->GetYaxis()->SetTitle("Probabilita'");
    
    // Crea un canvas per disegnare
    TCanvas *c1 = new TCanvas("c1", "Gaussian Distribution", 800, 600);
    
    // Disegna la funzione
    f1->Draw();
    
    // Salva il disegno
    c1->SaveAs("gaussian.png");
    c1->SaveAs("gaussian.root");
    
    std::cout << "Disegno salvato!" << std::endl;
    std::cout << "Canvas rimane aperto - puoi modificare il grafico interattivamente!" << std::endl;
    
    // Mantieni l'applicazione aperta per interazione
    app->Run();
}