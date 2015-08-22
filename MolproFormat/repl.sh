#!/bin/bash
cat SingleBasisSet.hpp | grep 'if(str==.*)' | sed -e 's/if(str=="\(.*\)")/if(regex_match(str,regex("\'\\'\\s*\1\'\\'\\s*")))/' -i SingleBasisSet.hpp

