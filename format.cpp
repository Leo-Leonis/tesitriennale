#include <TDirectory.h>
#include <TFile.h>
#include <TH1.h>
#include <TKey.h>
#include <iostream>

void DivideHistogramsByValue(std::string inputFileName,
                             std::string outputFileName) {

  // Apri il file .root in modalità lettura
  TFile *inputFile = TFile::Open(inputFileName.c_str(), "READ");
  if (!inputFile || inputFile->IsZombie()) {
    std::cout << "Errore: Impossibile aprire il file " << inputFileName.c_str()
              << '\n';
    return;
  }
  TH1F *h_nevs = (TH1F *)inputFile->Get("hevents");
  const double_t nevs = h_nevs->GetBinContent(1);

  // Crea un nuovo file per salvare i risultati
  TFile *outputFile = TFile::Open(outputFileName.c_str(), "RECREATE");

  // Lista delle chiavi nella directory principale
  TList *keys = inputFile->GetListOfKeys();
  if (!keys) {
    std::cout << "Errore: Nessuna chiave trovata nel file." << '\n';
    inputFile->Close();
    return;
  }

  // Itera su tutte le chiavi
  for (auto *keyObj : *keys) {
    TKey *key = dynamic_cast<TKey *>(keyObj);
    if (!key)
      continue;

    // Leggi l'oggetto associato alla chiave
    TObject *obj = key->ReadObj();

    // Controlla se è una directory
    if (obj->InheritsFrom(TDirectory::Class())) {
      TDirectory *dir = (TDirectory *)obj;
			outputFile->mkdir(dir->GetName()); // Crea una directory nel file di output
      outputFile->cd(dir->GetName()); // Passa alla directory appena creata

      // Lista di keys presente nella directory "dir"
      TList *histKeys = dir->GetListOfKeys();
      // Itera sugli oggetti nella directory
      for (auto *histKeyObj : *histKeys) {
        TKey *histKey = dynamic_cast<TKey *>(histKeyObj);
        if (!histKey)
          continue;

        TObject *histObj = histKey->ReadObj();
        if (histObj->InheritsFrom(TH1D::Class())) {
          // Se è un istogramma, dividilo per ratio_value
          TH1D *hist = (TH1D *)histObj;
					//TH1D* histClone = (TH1D*)hist->Clone(); // Clona l'istogramma per evitare problemi di ownership
          hist->Scale(1.0 / nevs, "width"); // Dividi per il valore

          // Salva l'istogramma modificato nel nuovo file
          hist->Write();
        }
      }

      outputFile->cd(); // Torna alla directory principale
    }
  }

  // Chiudi i file
  inputFile->Close();
  outputFile->Close();
}

int format() {
  std::vector<std::string> inputFileNames;
  //inputFileNames.push_back(std::string("simu_data/merged_modeA.root"));
  //inputFileNames.push_back(std::string("simu_data/merged_modeB.root"));
  //inputFileNames.push_back(std::string("simu_data/merged_modeD.root"));
  //inputFileNames.push_back(std::string("simu_data/merged_modeE.root"));
  //inputFileNames.push_back(std::string("simu_data/merged_modeF.root"));
  inputFileNames.push_back(std::string("simu_data/merged_modeG.root"));
  //inputFileNames.push_back(std::string("simu_data/merged_modeZ.root"));

  std::vector<std::string> outputFileNames;
  //outputFileNames.push_back(std::string("simu_data/main142_A_1e8.root"));
  //outputFileNames.push_back(std::string("simu_data/main142_B_1e8.root"));
  //outputFileNames.push_back(std::string("simu_data/main142_D_1e8.root"));
  //outputFileNames.push_back(std::string("simu_data/main142_E_1e8.root"));
  //outputFileNames.push_back(std::string("simu_data/main142_F_1e8.root"));
  outputFileNames.push_back(std::string("simu_data/main142_G_1e8.root"));
  //outputFileNames.push_back(std::string("simu_data/main142_OFF_1e8.root"));
  // double ratio_value = 981;

  const auto input_size = inputFileNames.size();
  if (input_size == outputFileNames.size()) {
    for (int i = 0; i != input_size; i++) {
      DivideHistogramsByValue(inputFileNames[i], outputFileNames[i]);
      std::cout << "debug" << '\n'; // debug
    }
  } else {
    std::cout << "LEO_ERROR: input size (" << input_size
              << ") and output size (" << outputFileNames.size()
              << ") are different" << '\n';
  }

  std::cout << "LEO_INFO: Elaborazione completata. File salvati" << '\n';
  return 0;
}
