#!/bin/bash
sed 's/if(str=="\(.*\)")/if(regex_match(str,regex("\'\\'\\s*\1\'\\'\\s*")))/g' SingleBasisSet.hpp > temporary

