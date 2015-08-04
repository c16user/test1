#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	A.importBasisSetGamessFormat("HCuNbad.basis");
	A.printBasisSet();
	A.exportBasisSetMolproFormat("3.basis");

	return 0;
}


