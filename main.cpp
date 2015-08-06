#include "SingleBasisSet.hpp"

int main(int argc, char** argv){

	SingleBasisSet<double> A;
	if(!A.importBasisSetGamessFormat(argv[1])) return 1;
	A.printBasisSet();
	A.exportBasisSetMolproFormat("3.basis");

	return 0;
}


