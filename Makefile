# Zunavision  MAKEFILE
# Sid Batra <sidbatra@cs.stanford.edu>

-include make.mk

#######################################################################
# add application and shared source files here
#######################################################################
APP_SRC = test.cpp

OTHER_SRC = point.cpp position.cpp tag.cpp frame.cpp video.cpp tracker.cpp constants.cpp logger.cpp utilities.cpp visionUtilities.cpp xmlParser.cpp svlObjectList.cpp

#######################################################################

APP_PROG_NAMES = $(APP_SRC:.cpp=)
APP_OBJ = $(APP_SRC:.cpp=.o)
OTHER_OBJ = $(OTHER_SRC:.cpp=.o)

.PHONY: clean
.PRECIOUS: $(APP_OBJ)

all: depend ${addprefix ${BIN_PATH}/,$(APP_PROG_NAMES)}

$(BIN_PATH)/%: %.o $(OTHER_OBJ)
	${CCC} $*.o -o $(@:.o=) $(OTHER_OBJ) $(LFLAGS)

.cpp.o:
	${CCC} ${CFLAGS} -c $< -o $@

depend:
	g++ ${CFLAGS} -MM ${APP_SRC} ${OTHER_SRC} >depend

clean:
	-rm $(APP_OBJ)
	-rm $(OTHER_OBJ)
	-rm ${addprefix ${BIN_PATH}/,$(APP_PROG_NAMES)}
	-rm depend

-include depend

