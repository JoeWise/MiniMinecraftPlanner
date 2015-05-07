# $Id: Makefile,v 1.3 2013-02-12 15:09:10-08 - - $

# By default, gmake sets the variable ${CXX}=g++ on a Linux system.
# To use Sun Studio C++ (CC), set the variable CXX=CC, either on
# the command line when calling make, or in an environment variable.

MKFILE      = Makefile
NOINCL      = ci clean spotless
GMAKE       = ${MAKE} --no-print-directory

COMPILECPP  = g++ -g -O0 -Wall -Wextra -std=gnu++11
MAKEDEPCPP  = g++ -MM

OTHERSRC    = rapidjson/*.h rapidjson/internal/*.h
CPPHEADER   = graph.h planner.h crafting.h
CPPSOURCE   = ${CPPHEADER:.h=.cpp} main.cpp
ALLCPPSRC   = ${CPPHEADER} ${CPPSOURCE} ${TCCSHARED}
OBJECTS     = ${CPPSOURCE:.cpp=.o}
EXECBIN     = planner
OTHERS      = ${MKFILE} README
ALLSOURCES  = ${ALLCPPSRC} ${OTHERS}



all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${COMPILECPP} -o $@ ${OBJECTS}

%.o : %.cpp Makefile
	${COMPILECPP} -c $<

ci : ${ALLSOURCES}
	- checksource ${ALLSOURCES}

clean :
	- rm ${OBJECTS}

spotless : clean
	- rm ${EXECBIN}

again :
	${GMAKE} spotless


