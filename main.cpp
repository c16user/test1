#include "SingleBasisSet.hpp"
//#include <iostream>
//#include <fstream>
//#include <cstdlib>

int main(int argc, char** argv){
/*
	std::cout<<"Число переданных аргументов программе: "<<argc<<'\n';
	for (int i=0; i<argc; i++)
		std::cout<<"Аргумент #"<<i<<": "<<argv[i]<<", а это смешной квадрат целого числа "<<atoi(argv[i])*atoi(argv[i])<<'\n';
	double * a = new double [80];
	std::cout<<a;
	delete [] a;*/
	
	SingleBasisSet<double> A;


	A.importBasisSetGamessFormat("HCuN.basis");
	A.printBasisSet();
	A.importBasisSetGamessFormat("vtz.basis");
	A.exportBasisSetMolproFormat("3.basis");

	return 0;
}


