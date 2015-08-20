#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	//if(!A.importBasisSetGamessFormat("HCuN.basis")) return 1;
	//A.printBasisSet();
	A.importBasisSetMolproFormat("HNiF.basis");
	A.exportBasisSetMolproFormat("1HNiF.basis");
	A.printBasisSet();
	return 0;
}


