//////////////////////////////////////////////////////////////////////////////
//Program that reads an analyze the ROOT file retourned by 
//a CORSIKA simulation
/////////////////////////////////////////////////////////////////////////////

/**
*Reads a CORSIKA .root file and saves the relevant data in a file
**/
void readFile(char* nameFile, bool headers) 
{
 //Load shared library
  gSystem->Load("libEG");
  gSystem->Load("libExRootAnalysis.so");
  gSystem->Load("libPhysics");


  //Load the root-tupla
  TFile *myfile = new TFile(nameFile,"READ");

  //Generated 5 classes in crsIO
  myfile->MakeProject("crsIO","*","RECREATE++"); 

  Double_t conv=180.0/TMath::Pi();
    //Accessing sim tree
    TTree *tree = (TTree*)myfile->Get("sim"); 
    //Definition class Tshower and accessing branch shower
    class TShower;
    crsIO::TShower *shower = 0;
    tree->SetBranchAddress("shower.",&shower); 
  
    //Definition class TParticle and accessing branch particle
    TClonesArray *particles;
    tree->SetBranchAddress("particle.",&particles);

    //////////////////////////////////////////////////////
    //StartAnalysis
    /////////////////////////////////////////////////////

    Int_t entries = tree->GetEntries();
    Int_t nparticle = 0;
    Double_t mass_muon = 105.6583745e-3 ; //[GeV/c] 
    //Open text file 
    ofstream data_prim_file;
    data_prim_file.open ("PrimaryData.txt", std::ofstream::out | std::ofstream::app);
    if(headers) {
      data_prim_file<<"Theta_prim\tPhi_prim\n";
    }
    
    ofstream data_muon_file;
    data_muon_file.open ("MuonsData.txt", std::ofstream::out | std::ofstream::app);
    if(headers) {
      data_muon_file<<"Theta\tPhi\tMomentum\tEnergy\tX\tY\n";
    }
    
    //Start loop over events
    for (int i = 0; i<entries; i++)
      {
      	tree->GetEntry(i);
      	data_prim_file << (shower->Theta)*conv << "\t" << (shower->Phi)*conv << "\n";
      
      	nparticle = particles->GetEntries();
      	 //Start loop over particles
      	for(int j = 0; j < nparticle; ++j) {
      	//Takes j particle
      	  crsIO::TParticle * particle = (crsIO::TParticle*)particles->At(j);
      	  if ((particle->ParticleID==5)||(particle->ParticleID==6)){
      	    //Calculates total momentum
      	    Double_t pz = particle->Pz;//[GeV/c]
      	    Double_t px = particle->Px;
      	    Double_t py = particle->Py;
      	    Double_t p = TMath::Sqrt(pz**2+px**2+py**2);
         
      	  
      	    Double_t energy = TMath::Sqrt((p)**2+(mass_muon)**2);
      	  
      	    Double_t theta =TMath::ACos(pz/p)*conv; //Calculates angle theta of incidence
      	    Double_t phi = TMath::ATan2(py,px)*conv; //Calc angle phi of incidence
      	  
      	    Double_t pos_x = particle->x;
      	    Double_t pos_y = particle->y;
      	  

      
      	    data_muon_file << theta  << "\t" << phi  << "\t" << p  << "\t" << energy  << "\t" << pos_x  << "\t" << pos_y  <<"\n";
      	}
      } // end loop over particles
     }//End loop events

    data_prim_file.close();
    data_muon_file.close();
    myfile->Close();
}

/**
* Writes all the data from files listed in filelist.txt
**/
void writeFile() {
  //Opens file list 
  ifstream in;
  in.open(Form("./filelist.txt"));
  TString fileName;
  bool headers = true;
  if (in.is_open()) {
    while (in.good()) {
      in >> fileName;
      readFile(fileName,headers);
      if(headers == true) {
        headers = false;
      }
      cout << "Proceced file: "<< fileName << "\n";
    }
    in.close();
  }
  else cout << "Unable to open filelist.txt";
}

/**
*Creates fit functions for program
**/

void createFitFunctions() {
	Double_t conv=TMath::Pi()/180.0;
	TF1 *fitCos2=new TF1("fitCos2","[0]*cos([1]*x)*cos([1]*x)",0,90);
	fitCos2->SetParameter(1,conv);
	TF1 *fitCos3=new TF1("fitCos3","[0]*(cos([1]*x)**3)*sin([1]*x)",0,90);
	fitCos3->SetParameter(1,conv);
}

/***********************************************************
 *Main program
 ***********************************************************/

void macroCORSIKAtoText()
{
  //Creates input stream for reading files
  ifstream muonsf;
  ifstream primaryf;
  
  muonsf.open(Form("MuonsData.txt"));
  primaryf.open(Form("PrimaryData.txt"));
  
  if (!(muonsf.good() && primaryf.good() )) { //Checks if writing data files is needed
    writeFile();   //Writes data files (muons and primary)
    muonsf.close();
    primaryf.close();
  }
   //Create a new Canvas
  TCanvas *c1= new TCanvas ("c1","c1");
  c1->Divide(1,2);
  c1->SetGrid();
  
  TCanvas *c2= new TCanvas ("c2","c2");
  c2->Divide(1,3);
  c2->SetGrid();
  
  TCanvas *c3 = new TCanvas ("c3","c3");
  c3->Divide(1,2);
  c3->SetGrid();
  
  TCanvas *c4 = new TCanvas ("c4","c4");
  c4->Divide(1,1);
  
  //Creates histograms
  TH1F *h_shower_phi = new TH1F("shower_phi","Shower Phi (Primary)",50,-200,200);
          h_shower_phi->GetXaxis()->SetTitle("Phi [Grad]");
		      h_shower_phi->GetYaxis()->SetTitle("# Angles ");

  TH1F *h_shower_theta = new TH1F("shower_theta","Shower Theta (Primary)",50,0,90);
          h_shower_theta->GetXaxis()->SetTitle("Theta [Grad]");
		      h_shower_theta->GetYaxis()->SetTitle("# Angles");
  
  TH1F *h_nmuons_theta = new TH1F("nmuons_theta","Muons vs Theta (a)",50,0,90);
          h_nmuons_theta->GetXaxis()->SetTitle("Theta [Grad]");
		      h_nmuons_theta->GetYaxis()->SetTitle("# Muons [m^-2 s^-1 sr^-1] ");

  TH1F *h_nmuons_phi = new TH1F("nmuons_phi","Muons vs Phi ",50,-200,200);
          h_nmuons_phi->GetXaxis()->SetTitle("Phi [Grad]");
		      h_nmuons_phi->GetYaxis()->SetTitle("# Muons [m^-2 s^-1 sr^-1] ");

  TH1F *h_nmuons_theta_distr = new TH1F("nmuons_theta_distr","Muons vs Theta (b)",50,0,90);  
          h_nmuons_theta_distr->GetXaxis()->SetTitle("Theta [Grad]");
		      h_nmuons_theta_distr->GetYaxis()->SetTitle("# Muons [m^-2 s^-1] ");

  TH1F *h_nmuons_momentum = new TH1F("nmuons_momentum","# Muons vs Total Momentum [GeV/c]",50,0,20);
          h_nmuons_momentum->GetXaxis()->SetTitle("Momentum [GeV/c]");
		      h_nmuons_momentum->GetYaxis()->SetTitle("# Muons [m^-2 s^-1 sr^-1] ");

  TH1F *h_nmuons_energy = new TH1F("nmuons_energy","# Muons vs Total Energy [GeV]",50,0,20);
          h_nmuons_energy->GetXaxis()->SetTitle("Energy [GeV]");
		      h_nmuons_energy->GetYaxis()->SetTitle("# Muons [m^-2 s^-1 sr^-1] ");

  TH2F *h_nmuons_xy = new TH2F("nmuons_xy","Muons xy position infty",50,-1e5,1e5,50,-1e5,1e5);
          h_nmuons_xy->GetXaxis()->SetTitle("x [cm]");
		      h_nmuons_xy->GetXaxis()->SetTitleOffset(1);
          h_nmuons_xy->GetYaxis()->SetTitle("y [cm]"); 
		      h_nmuons_xy->GetYaxis()->SetTitleOffset(1);
          h_nmuons_xy->GetZaxis()->SetTitle("# Muons"); 

  TH2F *h_nmuons_xy_filtered = new TH2F("nmuons_xy_filtered","Muons xy position 25cmx25cm",50,-1e5,1e5,50,-1e5,1e5);
          h_nmuons_xy_filtered->GetXaxis()->SetTitle("x [cm]");
		      h_nmuons_xy_filtered->GetXaxis()->SetTitleOffset(2);
          h_nmuons_xy_filtered->GetYaxis()->SetTitle("y [cm]"); 
		      h_nmuons_xy_filtered->GetYaxis()->SetTitleOffset(2);
          h_nmuons_xy_filtered->GetZaxis()->SetTitle("# Muons"); 
  
  //Variables to save data
  //Primary
  Double_t theta_p;
  Double_t phi_p;
  string headers_p;
  //Muons
  Double_t theta_m;
  Double_t phi_m;
  Double_t momentum_m;
  Double_t energy_m;
  Double_t pos_x_m;
  Double_t pos_y_m;
  Double_t conv =  TMath::Pi()/180.0;
  string headers_m;
  
  getline(primaryf,headers_p);
  while(primaryf.good()) { //Loop over primary data
        primaryf >> theta_p >> phi_p;
        h_shower_phi -> Fill(phi_p,1);
        h_shower_theta -> Fill(theta_p,1);
  } //End loop primary data              

  getline(muonsf,headers_m);
  while(muonsf.good()) { //Loop over muon data
        muonsf >> theta_m >> phi_m >>momentum_m>>energy_m>>pos_x_m>>pos_y_m;
        
        h_nmuons_theta_distr -> Fill(theta_m,1);
        h_nmuons_phi -> Fill(phi_m,1);
        
        Double_t cos = TMath::Cos(theta_m*conv);
        Double_t sin = TMath::Sin(theta_m*conv);
        h_nmuons_theta-> Fill(theta_m,1/(cos*sin));
        
        h_nmuons_momentum ->Fill(momentum_m,1);
        h_nmuons_energy -> Fill(energy_m);
        h_nmuons_xy -> Fill(pos_x_m,pos_y_m,1);
        if( (pos_x_m<=50) && (pos_x_m>=-50) && (pos_y_m<=50) && (pos_y_m>=-50) ) {	//FIltering of position  
	        h_nmuons_xy_filtered->Fill(pos_x_m,pos_y_m,1);
        }
  } //End loop muon data 

    createFitFunctions();
  
    gStyle->SetOptFit(1101);
  //Drawing the histograms
  c1->cd(1);
    h_shower_phi->Draw();
    c1_1->SetGridx();
    c1_1->SetGridy();
  c1->cd(2);
    h_nmuons_phi->Draw();
   c1_2->SetGridx();
    c1_2->SetGridy();
  
  c2->cd(1);
    h_shower_theta->Draw();
    h_shower_theta->Fit("gaus");
   c2_1->SetGridx();
   c2_1->SetGridy();
  
  c2->cd(2);
   c2_2->SetGridx();
    c2_2->SetGridy();
   h_nmuons_theta_distr->Fit("fitCos3");
   h_nmuons_theta_distr->Sumw2();
   h_nmuons_theta_distr->Draw("E");
  c2->cd(3);
   c2_3->SetGridx();
    c2_3->SetGridy();
   h_nmuons_theta->Fit("fitCos2");
   h_nmuons_theta->Sumw2();
   h_nmuons_theta->Draw("E");
 
  c3->cd(1);
   c3_1->SetGridx();
    c3_1->SetGridy();
   h_nmuons_momentum->Draw();
  c3->cd(2);
   c3_2->SetGridx();
    c3_2->SetGridy();
   h_nmuons_energy->Draw();

   c4->cd(1);
   c4_1->SetGridx();
    c4_1->SetGridy();
   h_nmuons_xy->Draw();
  
  //Close data files
  muonsf.close();
  primaryf.close();
  
 c1->Modified();
 c2->Modified();
 c3->Modified();
 c4->Modified();

 c1->Print("Phi_2H.eps");
 c2->Print("Theta_2H.eps");
 c3->Print("Energy_2H.eps");
 c4->Print("XY_2H.eps");
} //end program




