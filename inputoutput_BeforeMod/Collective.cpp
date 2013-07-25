/***************************************************************************
                          Collective.h  -  description
                             -------------------
    begin                : Wed Jun 2 2004
    copyright            : (C) 2004 Los Alamos National Laboratory
    developers           : Stefano Markidis, Giovanni Lapenta
    email                : markidis@lanl.gov, lapenta@lanl.gov
 ***************************************************************************/



/**
*  Collective properties. Input physical parameters for the simulation.
*  Use KVF to parse the input file
*
* @date Fri Jun 4 2004
* @par Copyright:
* (C) 2004 Los Alamos National Laboratory
* @author Stefano Markidis, Giovanni Lapenta
* @version 1.0
*/
#include <iostream>
#include <string.h>
#include <stdlib.h>

#include <math.h>
#include "Collective.h"
#include "kvfDataSource.h"
// use hdf5 for the restart file
#include "hdf5.h"




using std::cout;
using std::endl;
/** Read the input file from text file and put the data in a collective wrapper:
    if it's a restart read from input file basic sim data and load particles and EM field
    from restart file */
void Collective::ReadInput(string inputfile){
  using namespace std;
  using namespace KVF;

  int i_val;
  vector<int> i_vvals;
  vector<double> d_vvals;

  try{
    //cout << "About to open file: "<< inputfile << endl;
   KVFDataSource kv_file(inputfile );

   if( kv_file.num_parse_errors() != 0 )
   {
	cout << "THERE WERE ERRORS READING THE FILE!! " << endl;
	kv_file.diag_print_errors() ;
	exit(1);
   }


   // the following variables are ALWAYS taken from inputfile, even if restarting
   {
        kv_file.get_data( "dt", dt );
	    kv_file.get_data( "ncycles", ncycles );
	    kv_file.get_data( "th", th );
	    kv_file.get_data( "Smooth", Smooth );
	    kv_file.get_data( "Nvolte", Nvolte );
	    kv_file.get_data( "XLEN", XLEN );
	    kv_file.get_data( "YLEN", YLEN );
	    kv_file.get_data( "ngrids", ngrids );
	    kv_file.get_data( "ratio", ratio );
        kv_file.get_data( "SaveDirName",  SaveDirName );
	cout << "From within Collective: SaveDirName: " << SaveDirName <<endl;
	    kv_file.get_data( "RestartDirName",  RestartDirName );
		kv_file.get_data( "ns", ns );
	    kv_file.get_data( "NpMaxNpRatio", NpMaxNpRatio);
	    // GEM Challenge
	    kv_file.get_data( "B0x", B0x );
	    kv_file.get_data( "B0y", B0y );
	    kv_file.get_data( "B0z", B0z );
        kv_file.get_data( "delta", delta );
	    rhoINIT = new double[ns];
		kv_file.get_data( "rhoINIT", d_vvals );
	    for (register int i=0; i<ns; i++)
		   rhoINIT[i]=d_vvals[i];
	    // take the tolerance of the solvers
		kv_file.get_data( "CGtol", CGtol);
		kv_file.get_data( "GMREStol", GMREStol);
		kv_file.get_data( "NiterMover", NiterMover);
		// take the injection of the particless
		kv_file.get_data( "Vinj", Vinj);
		// take the output cycles
		kv_file.get_data( "FieldOutputCycle",FieldOutputCycle);
		kv_file.get_data( "ParticlesOutputCycle",ParticlesOutputCycle);
		kv_file.get_data( "RestartOutputCycle",RestartOutputCycle);

    }

    if (RESTART1){    // you are restarting
       kv_file.get_data( "RestartDirName",  RestartDirName );
       ReadRestart(RestartDirName);
    }
    else
    {
       restart_status=0;
       last_cycle=-1;
       kv_file.get_data( "c", c );
       kv_file.get_data( "Lx", Lx );
       kv_file.get_data( "Ly", Ly );
       kv_file.get_data( "nxc", nxc );
       kv_file.get_data( "nyc", nyc );


       npcelx = new int[ns];
       npcely = new int[ns];
       qom = new double[ns];
       uth = new double[ns];
       vth = new double[ns];
       wth = new double[ns];
       u0 = new double[ns];
       v0 = new double[ns];
       w0 = new double[ns];

	   kv_file.get_data( "npcelx", i_vvals );
	   for (register int i=0; i<ns; i++)
		  npcelx[i]=i_vvals[i];
	   kv_file.get_data( "npcely", i_vvals );
	   for (register int i=0; i<ns; i++)
		  npcely[i]=i_vvals[i];
	   kv_file.get_data( "qom", d_vvals );
	   for (register int i=0; i<ns; i++)
		 qom[i]=d_vvals[i];
	   kv_file.get_data( "uth", d_vvals );
	   for (register int i=0; i<ns; i++)
		 uth[i]=d_vvals[i];
	   kv_file.get_data( "vth", d_vvals );
	   for (register int i=0; i<ns; i++)
		 vth[i]=d_vvals[i];
	kv_file.get_data( "wth", d_vvals );
	for (register int i=0; i<ns; i++)
		wth[i]=d_vvals[i];
	kv_file.get_data( "u0", d_vvals );
	for (register int i=0; i<ns; i++)
		u0[i]=d_vvals[i];
	kv_file.get_data( "v0", d_vvals );
	for (register int i=0; i<ns; i++)
		v0[i]=d_vvals[i];
	kv_file.get_data( "w0", d_vvals );
	for (register int i=0; i<ns; i++)
		w0[i]=d_vvals[i];

	kv_file.get_data( "verbose", i_val);


	if (i_val==0)
		verbose=false;
		else
		verbose=true;
	kv_file.get_data( "bcPHIfaceXright", bcPHIfaceXright );
	kv_file.get_data( "bcPHIfaceXleft" , bcPHIfaceXleft );
	kv_file.get_data( "bcPHIfaceYright", bcPHIfaceYright );
	kv_file.get_data( "bcPHIfaceYleft",  bcPHIfaceYleft );
	kv_file.get_data( "bcEMfaceXright", bcEMfaceXright );
	kv_file.get_data( "bcEMfaceXleft" , bcEMfaceXleft );
	kv_file.get_data( "bcEMfaceYright", bcEMfaceYright );
	kv_file.get_data( "bcEMfaceYleft",  bcEMfaceYleft );
	kv_file.get_data( "bcPfaceXright", bcPfaceXright );
	kv_file.get_data( "bcPfaceXleft" , bcPfaceXleft );
	kv_file.get_data( "bcPfaceYright", bcPfaceYright );
	kv_file.get_data( "bcPfaceYleft",  bcPfaceYleft );


      }

	TrackParticleID =  new bool[ns];
	kv_file.get_data( "TrackParticleID", i_vvals);
	for (register int i=0; i<ns; i++){
		if (i_vvals[i]==0)
		TrackParticleID[i]=false;
		else
		TrackParticleID[i]=true;}

        /**AMR variables, ME*/
        kv_file.get_data( "PRA_Xleft",  PRA_Xleft );
        kv_file.get_data( "PRA_Xright",  PRA_Xright );
        kv_file.get_data( "PRA_Yleft",  PRA_Yleft );
        kv_file.get_data( "PRA_Yright",  PRA_Yright );


}
catch( KVFException& e )
{
	cout << "Collective::ReadInput() Caught exception " << endl;
	e.diag_cout();
}

}
/**
*  Read the collective information from the RESTART file in HDF5 format
*
*  There are three restart status:
*  restart_status = 0 --->  new inputfile
*  restart_status = 1 --->  RESTART and restart and result directories does not coincide
*  restart_status = 2 --->  RESTART and restart and result directories coincide
*
*/
int Collective::ReadRestart(string inputfile){

    restart_status=1;
    // hdf stuff
    hid_t    file_id;
    hid_t    dataset_id;
    herr_t   status;
    /*
     * Open the  setting file for the restart.
     */
     file_id = H5Fopen((inputfile+"/settings.hdf").c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
     if (file_id < 0){
        cout << "couldn't open file: " << inputfile << endl;\
	return -1;}

     // read c
    dataset_id = H5Dopen1(file_id, "/collective/c");
    status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&c);
    status = H5Dclose(dataset_id);

     // read Lx
     dataset_id = H5Dopen1(file_id, "/collective/Lx");
     status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&Lx);
     status = H5Dclose(dataset_id);
     // read Ly
     dataset_id = H5Dopen1(file_id, "/collective/Ly");
     status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&Ly);
     status = H5Dclose(dataset_id);
     // read nxc
     dataset_id = H5Dopen1(file_id, "/collective/Nxc");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&nxc);
     status = H5Dclose(dataset_id);
     // read nyc
     dataset_id = H5Dopen1(file_id, "/collective/Nyc");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&nyc);
     status = H5Dclose(dataset_id);
     // read ns
     dataset_id = H5Dopen1(file_id, "/collective/Ns");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&ns);
     status = H5Dclose(dataset_id);
     // read ngrids
     dataset_id = H5Dopen1(file_id, "/collective/Ngrids");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&ngrids);
     status = H5Dclose(dataset_id);
     // read XLEN
     dataset_id = H5Dopen1(file_id, "/collective/XLEN");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&XLEN);
     status = H5Dclose(dataset_id);
     // read YLEN
     dataset_id = H5Dopen1(file_id, "/collective/YLEN");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&YLEN);
     status = H5Dclose(dataset_id);


     /** Boundary condition information */
     // read EMfaceXleft
     dataset_id = H5Dopen1(file_id, "/collective/bc/EMfaceXleft");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcEMfaceXleft);
     status = H5Dclose(dataset_id);
     // read EMfaceXright
     dataset_id = H5Dopen1(file_id, "/collective/bc/EMfaceXright");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcEMfaceXright);
     status = H5Dclose(dataset_id);
     // read EMfaceYleft
     dataset_id = H5Dopen1(file_id, "/collective/bc/EMfaceYleft");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcEMfaceYleft);
     status = H5Dclose(dataset_id);
     // read EMfaceYright
     dataset_id = H5Dopen1(file_id, "/collective/bc/EMfaceYright");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcEMfaceYright);
     status = H5Dclose(dataset_id);

     // read PHIfaceXleft
     dataset_id = H5Dopen1(file_id, "/collective/bc/PHIfaceXleft");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPHIfaceXleft);
     status = H5Dclose(dataset_id);
     // read PHIfaceXright
     dataset_id = H5Dopen1(file_id, "/collective/bc/PHIfaceXright");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPHIfaceXright);
     status = H5Dclose(dataset_id);
     // read PHIfaceYleft
     dataset_id = H5Dopen1(file_id, "/collective/bc/PHIfaceYleft");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPHIfaceYleft);
     status = H5Dclose(dataset_id);
     // read PHIfaceYright
     dataset_id = H5Dopen1(file_id, "/collective/bc/PHIfaceYright");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPHIfaceYright);
     status = H5Dclose(dataset_id);

     // read PfaceXleft
     dataset_id = H5Dopen1(file_id, "/collective/bc/PfaceXleft");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPfaceXleft);
     status = H5Dclose(dataset_id);
     // read PfaceXright
     dataset_id = H5Dopen1(file_id, "/collective/bc/PfaceXright");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPfaceXright);
     status = H5Dclose(dataset_id);
     // read PfaceYleft
     dataset_id = H5Dopen1(file_id, "/collective/bc/PfaceYleft");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPfaceYleft);
     status = H5Dclose(dataset_id);
     // read PfaceYright
     dataset_id = H5Dopen1(file_id, "/collective/bc/PfaceYright");
     status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&bcPfaceYright);
     status = H5Dclose(dataset_id);


     // allocate fields depending on species
     npcelx = new int[ns];
     npcely = new int[ns];
     qom = new double[ns];
     uth = new double[ns];
     vth = new double[ns];
     wth = new double[ns];
     u0 = new double[ns];
     v0 = new double[ns];
     w0 = new double[ns];

     // read data  from species0, species 1, species2,...
     string* name_species = new string[ns];

     stringstream *ss = new stringstream[ns];



     for (int i=0;i <ns;i++){
        ss[i] << i;
	name_species[i] = "/collective/species_"+ ss[i].str() +"/";

     }

     // npcelx for different species
     for(int i=0;i < ns;i++){
         dataset_id = H5Dopen1(file_id, (name_species[i]+"Npcelx").c_str());
         status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&npcelx[i]);
         status = H5Dclose(dataset_id);
     }

     // npcely for different species
     for(int i=0;i < ns;i++){
         dataset_id = H5Dopen1(file_id, (name_species[i]+"Npcely").c_str());
         status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&npcely[i]);
         status = H5Dclose(dataset_id);
     }

     // qom for different species
     for(int i=0;i < ns;i++){
         dataset_id = H5Dopen1(file_id, (name_species[i]+"qom").c_str());
         status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&qom[i]);
         status = H5Dclose(dataset_id);
     }
     /** not needed for restart **/
     for (int i=0; i<ns; i++)
		uth[i]=0.0;
     for (int i=0; i<ns; i++)
		vth[i]=0.0;
     for (int i=0; i<ns; i++)
		wth[i]=0.0;
     for (int i=0; i<ns; i++)
		u0[i]=0.0;
     for (int i=0; i<ns; i++)
		v0[i]=0.0;
     for (int i=0; i<ns; i++)
		w0[i]=0.0;
     // verbose on
     verbose = 1;


     // if RestartDirName == SaveDirName overwrite dt,Th,Smooth (append to old hdf files)
     if (RestartDirName == SaveDirName){
         restart_status=2;
         // read dt
         dataset_id = H5Dopen1(file_id, "/collective/Dt");
         status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&dt);
         status = H5Dclose(dataset_id);
         // read th
         dataset_id = H5Dopen1(file_id, "/collective/Th");
         status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&th);
         status = H5Dclose(dataset_id);
         // read Smooth
         dataset_id = H5Dopen1(file_id, "/collective/Smooth");
         status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT,&Smooth);
         status = H5Dclose(dataset_id);
         // read Nvolte
         dataset_id = H5Dopen1(file_id, "/collective/Nvolte");
         status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&Nvolte);
         status = H5Dclose(dataset_id);
      }

     status = H5Fclose(file_id);
     delete[] name_species;

     // read last cycle (not from settings, but from restart0.hdf)

     file_id = H5Fopen((inputfile+"/restart0.hdf").c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
     if (file_id < 0){
        cout << "couldn't open file: " << inputfile << endl;\
	return -1;}

    dataset_id = H5Dopen1(file_id, "/last_cycle");
    status = H5Dread(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,&last_cycle);
    status = H5Dclose(dataset_id);
    status = H5Fclose(file_id);

}

Collective::Collective(int argc, char** argv) {

  if( argc < 2 ){
   inputfile = "inputfile";
   RESTART1=false;}
  else if ( argc < 3 ){
    inputfile = argv[1];
    RESTART1=false;}
  else{
      if( strcmp(argv[1],"restart")==0 ){
        inputfile = argv[2];
        RESTART1=true;}
       else if ( strcmp(argv[2],"restart")==0  ){
         inputfile = argv[1];
         RESTART1=true;}
   else{
    cout<<"Error: syntax error in mpirun arguments. Did you mean to 'restart' ?"<<endl;
    return;}
  }

    ReadInput(inputfile);

    /** fourpi = 4 greek pi */
    fourpi = 16.0*atan(1.0);

    /** dx = space step - X direction */
    dx = Lx/ (double) nxc;
    /** dy = space step - Y direction */
    dy = Ly/ (double) nyc;

    /** npcelx = number of particles per cell - Direction X
    <ul>
    	<li> npcelx[0] = for species 0</li>
    	<li> npcelx[1] = for species 1</li>
    </ul>

    */
    /** npcel = number of particles per cell */
    /** np = number of particles of different species */
    /** npMax = maximum number of particles of different species */

    npcel = new int[ns];
    np = new int[ns];
    npMax = new int[ns];

    for (int i=0; i<ns; i++){
      npcel[i] = npcelx[i]*npcely[i];
      np[i] = npcel[i]*nxc*nyc;
      npMax[i] = (int) (NpMaxNpRatio*np[i]);
    }



}
/** destructor */
Collective::~Collective(){
  delete[] np;
  delete[] npcel;
  delete[] npcelx;
  delete[] npcely;
  delete[] npMax;
  delete[] qom;

  delete[] uth;
  delete[] vth;
  delete[] wth;

  delete[] u0;
  delete[] v0;
  delete[] w0;

  delete[] rhoINIT;

}
/** Print Simulation Parameters */
void Collective::Print(){
  cout << endl;
  cout << "Simulation Parameters" << endl;
  cout << "---------------------" << endl;
  cout << "Number of species    = " << ns << endl;
  for (int i=0; i < ns;i++)
    cout << "Number of superparticles of species " << i << " = " << np[i] << "\t (MAX = " << npMax[i] << ")" << endl;

  cout << "x-Length                 = " << Lx      << endl;
  cout << "y-Length                 = " << Ly      << endl;
  cout << "Number of grids          = " << ngrids  << endl;
  cout << "Number of cells (x)      = " << nxc     << endl;
  cout << "Number of cells (y)      = " << nyc     << endl;
  cout << "Number of processors in X = " << XLEN << endl;
  cout << "Number of processors in Y = " << YLEN << endl;
  cout << "Time step                = " << dt      << endl;
  cout << "Number of cycles         = " << ncycles << endl;
  cout << "Results saved in: "<< SaveDirName <<endl;
  cout << "---------------------" << endl;
  cout << "Check Simulation Constraints" << endl;
  cout << "---------------------" << endl;
  cout << "Accuracy Constraint:  " << endl;
  for (int i=0; i < ns;i++){
    cout << "u_th < dx/dt species " << i << "....." ;
    if (uth[i] < (dx/dt))
      cout << "OK" << endl;
    else
      cout << "NOT SATISFIED. STOP THE SIMULATION." << endl;

    cout << "v_th < dy/dt species " << i << "......" ;
    if (vth[i] < (dy/dt))
      cout << "OK" << endl;
    else
      cout << "NOT SATISFIED. STOP THE SIMULATION." << endl;
  }
  cout << endl;
  cout << "Finite Grid Stability Constraint:  ";
  cout << endl;
  for (int is=0; is < ns;is++){
   if (uth[is]*dt/dx > .1)
     cout << "OK u_th*dt/dx (species " << is << ") = "  << uth[is]*dt/dx << " > .1" << endl;
   else
     cout << "WARNING.  u_th*dt/dx (species "<< is <<") = "  << uth[is]*dt/dx << " < .1" << endl;

   if (vth[is]*dt/dy > .1)
     cout << "OK v_th*dt/dy (species " << is <<") = " << vth[is]*dt/dy << " > .1" << endl;
   else
     cout << "WARNING. v_th*dt/dy (species "<< is << ") = "<< vth[is]*dt/dy << " < .1" << endl;

  }

}
/** get the physical space dimensions            */
int Collective::getDim(){
 return(dim);
}
/** get Lx */
double Collective::getLx(){
 return(Lx);
}
/** get Ly */
double Collective::getLy(){
 return(Ly);
}
/** get nxc */
int Collective::getNxc(){
 return(nxc);
}
/** get nyx */
int Collective::getNyc(){
  return(nyc);
}
/** get dx */
double Collective::getDx(){
  return(dx);
}
/** get dy */
double Collective::getDy(){
  return(dy);
}
/** get the light speed */
inline double Collective::getC(){
 return(c);
}
/** get the time step */
double Collective::getDt(){
 return(dt);
}
/** get the decentering parameter */
double Collective::getTh(){
 return(th);
}
/** get the smoothing parameter */
double Collective::getSmooth(){
	return(Smooth);
}
/** get the smoothing times */
int Collective::getNvolte(){
	return(Nvolte);
}
/** get the number of time cycles */
int Collective::getNcycles(){
 return(ncycles);
}
/** get the number of grids */
int Collective::getNgrids(){
 return(ngrids);
}
/** get the number of processors - x direction */
int Collective::getXLEN(){
 return(XLEN);
}
/** get the number of processors - x direction */
int Collective::getYLEN(){
 return(YLEN);
}
/** get the size ratio between grids */
int Collective::getRatio(){
 return(ratio);
}
/** get the number of species */
int Collective::getNs(){
 return(ns);
}
/** get the number of particles per cell for species nspecies */
int Collective::getNpcel(int nspecies){
 return(npcel[nspecies]);
}
/** get the number of particles per cell for species nspecies - direction X */
int Collective::getNpcelx(int nspecies){
 return(npcelx[nspecies]);
}
/** get the number of particles per cell for species nspecies - direction Y */
int Collective::getNpcely(int nspecies){
 return(npcely[nspecies]);
}

/** get the number of particles for different species */
int Collective::getNp(int nspecies){
 return(np[nspecies]);
}
/** get maximum number of particles for different species */
int Collective::getNpMax(int nspecies){
 return(npMax[nspecies]);
}
double Collective::getNpMaxNpRatio(){
  return(NpMaxNpRatio);
}
/** get charge to mass ratio for different species */
double Collective::getQOM(int nspecies){
 return(qom[nspecies]);
}
/** get the background density for GEM challenge */
double Collective::getRHOinit(int nspecies){
 return(rhoINIT[nspecies]);
}
/** get thermal velocity  - Direction X     */
double Collective::getUth(int nspecies){
 return(uth[nspecies]);
}
/** get thermal velocity  - Direction Y     */
double Collective::getVth(int nspecies){
 return(vth[nspecies]);
}
/** get thermal velocity  - Direction Z     */
double Collective::getWth(int nspecies){
return(wth[nspecies]);
}
/** get beam velocity - Direction X        */
double Collective::getU0(int nspecies){
 return(u0[nspecies]);
}
/** get beam velocity - Direction Y        */
double Collective::getV0(int nspecies){
 return(v0[nspecies]);
}
/** get beam velocity - Direction Z        */
double Collective::getW0(int nspecies){
return(w0[nspecies]);
}
/** get Boundary Condition Particles: FaceXright */
int Collective::getBcPfaceXright(){
 return(bcPfaceXright);
}
/** get Boundary Condition Particles: FaceXleft */
int Collective::getBcPfaceXleft(){
 return(bcPfaceXleft);
}
/** get Boundary Condition Particles: FaceYright */
int Collective::getBcPfaceYright(){
 return(bcPfaceYright);
}
/** get Boundary Condition Particles: FaceYleft */
int Collective::getBcPfaceYleft(){
 return(bcPfaceYleft);
}
/** get Boundary Condition Electrostatic Potential: FaceXright */
int Collective::getBcPHIfaceXright(){
 return(bcPHIfaceXright);
}
/** get Boundary Condition Electrostatic Potential:FaceXleft */
int Collective::getBcPHIfaceXleft(){
 return(bcPHIfaceXleft);
}
/** get Boundary Condition Electrostatic Potential:FaceYright */
int Collective::getBcPHIfaceYright(){
 return(bcPHIfaceYright);
}
/** get Boundary Condition Electrostatic Potential:FaceYleft */
int Collective::getBcPHIfaceYleft(){
 return(bcPHIfaceYleft);
}
/** get Boundary Condition EM Field: FaceXright */
int Collective::getBcEMfaceXright(){
 return(bcEMfaceXright);
}
/** get Boundary Condition EM Field: FaceXleft */
int Collective::getBcEMfaceXleft(){
 return(bcEMfaceXleft);
}
/** get Boundary Condition EM Field: FaceYright */
int Collective::getBcEMfaceYright(){
 return(bcEMfaceYright);
}
/** get Boundary Condition EM Field: FaceYleft */
int Collective::getBcEMfaceYleft(){
 return(bcEMfaceYleft);
}

/** Get GEM Challenge parameters */
double Collective::getDelta(){
  return(delta);
}
 double Collective::getB0x(){
  return(B0x);
}
 double Collective::getB0y(){
  return(B0y);
}
 double Collective::getB0z(){
  return(B0z);
}
/** get the boolean value for verbose results */
 bool Collective::getVerbose(){
  return(verbose);
}
/** get the boolean value for TrackParticleID */
 bool Collective::getTrackParticleID(int nspecies){
  return(TrackParticleID[nspecies]);
   }
 int Collective::getRestart_status(){
  return(restart_status);
 }
 /** get SaveDirName  */
 string Collective::getSaveDirName(){
   cout << "From inside getSaveDirName: "<< SaveDirName <<endl;
 return(SaveDirName);
 }
 /** get RestartDirName  */
 string Collective::getRestartDirName(){
 return(RestartDirName);
 }
 /** get inputfile  */
 string Collective::getinputfile(){
 return(inputfile);
  }
/** get last_cycle  */
int Collective::getLast_cycle(){
 return(last_cycle);
}
/** get the velocity of injection of the plasma from the wall */
double Collective::getVinj(){
 return(Vinj);
}
/** get the converging tolerance for CG solver */
double Collective::getCGtol(){
 return(CGtol);
}
/** get the converging tolerance for GMRES solver */
double Collective::getGMREStol(){
 return(GMREStol);
}
/** get the numbers of iteration for the PC mover */
int Collective::getNiterMover(){
 return(NiterMover);
}
/** output of fields */
int Collective::getFieldOutputCycle(){
 return(FieldOutputCycle);
}
/** output of fields */
int Collective::getParticlesOutputCycle(){
  return(ParticlesOutputCycle);
}
/** output of fields */
int Collective::getRestartOutputCycle(){
  return(RestartOutputCycle);
}

/**AMR variables, ME*/
/**get number of cells, ghost cell INCLUDED, for particle repopulation; x left*/
int Collective::GetPRA_Xleft(){
  return(PRA_Xleft);
}
/**get number of cells, ghost cell INCLUDED, for particle repopulation; x right*/
int Collective::GetPRA_Xright(){
  return(PRA_Xright);
}
/**get number of cells, ghost cell INCLUDED, for particle repopulation; y left*/
int Collective::GetPRA_Yleft(){
  return(PRA_Yleft);
}
/**get number of cells, ghost cell INCLUDED, for particle repopulation; y right*/
int Collective::GetPRA_Yright(){
  return(PRA_Yright);
}
