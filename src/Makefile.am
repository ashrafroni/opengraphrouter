AM_CPPFLAGS = -I$(srcdir)/../../../library/boost_rtree -I$(srcdir)/../../../library -I/usr/local/include/gdal
#bin_PROGRAMS = opengraphrouter RtreeIndexer
#opengraphrouter_SOURCES=opengraphrouter.cpp RtreeIndexer.cpp GraphManager.cpp LabelManager.cpp
bin_PROGRAMS = opengraphrouter 
opengraphrouter_SOURCES=opengraphrouter.cpp \
    BiDirectionalAStar.cpp \
    ConfigurationManager.cpp \
    DataBaseFieldIndexManager.cpp \
    GraphFileManager.cpp \
    GraphManager.cpp \
    LabelManager.cpp \
    MassDataConverter.cpp \
    PlainShortestPathGenerator.cpp \
    PointShapeCreator.cpp \
    RequestManager.cpp \
    ShortestPathAStar.cpp \
    ShortestPathBGL.cpp \
    ShortestPathBiDirectional.cpp \
    ShortestPathDikjstra.cpp \
    ShortestPathGenerator.cpp \
    TextBasedDrivingDirectionGenerator.cpp \
    TimerInitiator.cpp \
    TimeScheduleHandler.cpp

opengraphrouter_LDADD=$(LIBS) -lstdc++


