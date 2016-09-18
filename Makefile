.PHONY:

ifndef OUT
OUT=./pc
endif

CMP=g++
WARN=-Wall -Wno-unused-result -Wno-deprecated-declarations -Wno-sign-compare -Wno-maybe-uninitialized
OPTIM=-Ofast -march=native -funroll-loops -funsafe-loop-optimizations -falign-functions=16 -falign-loops=16 -fopenmp
NOOPTIM=-O0 -march=native -fopenmp
DBG=-g ${NOOPTIM}

INC=-DIL_STD -I/media/filippo/ssd/cplex/cplex/include -I/media/filippo/ssd/cplex/concert/include
LDIR=-L/media/filippo/ssd/cplex/concert/lib/x86-64_linux/static_pic -L/media/filippo/ssd/cplex/cplex/lib/x86-64_linux/static_pic
LINK=-lconcert -lilocplex -lcplex

COBJSUBDIR=cobj
DEPSUBDIR=dep

ECHOCC=echo "[\033[01;33m CC \033[0m]"
ECHOLD=echo "[\033[01;36m LD \033[0m]"

OPT=${NOOPTIM} # Put desired optimisation level here

define compilec
${ECHOCC} $(notdir $<) ;\
mkdir -p ${DEPSUBDIR} ;\
tmp=`mktemp` ;\
${CMP} ${DEFS} ${INC} -MM ${OPT} $< >> $$tmp ;\
if [ $$? -eq 0 ] ;\
then echo -n "${COBJSUBDIR}/" > ${DEPSUBDIR}/$(notdir $<).d ;\
cat $$tmp >> ${DEPSUBDIR}/$(notdir $<).d ;\
rm $$tmp ;\
mkdir -p ${COBJSUBDIR} ;\
cd ${COBJSUBDIR} ;\
${CMP} ${DEFS} -c ${INC} ${OPT} ${WARN} ../$< ;\
else \
ret=$$? ;\
rm $$tmp ;\
exit $$ret ;\
fi
endef

all: pc

-include ${DEPSUBDIR}/*.d

pc: ${COBJSUBDIR}/pc.o ${COBJSUBDIR}/io.o ${COBJSUBDIR}/sp.o ${COBJSUBDIR}/value.o
	@${ECHOLD} pc
	@${CMP} ${OPT} ${LDIR} $^ ${LINK} -o ${OUT}

${COBJSUBDIR}/value.o: value.cpp
	@$(compilec)

${COBJSUBDIR}/io.o: io.cpp
	@$(compilec)

${COBJSUBDIR}/sp.o: sp.cpp
	@$(compilec)

${COBJSUBDIR}/pc.o: pc.cpp
	@$(compilec)

clean:
	@echo "Removing subdirectories..."
	@rm -rf ${COBJSUBDIR} ${DEPSUBDIR}

run:
	./pc.sh test.sol
