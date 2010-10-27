CC = g++
CGLAGS = -w

SRCS = AudioNode.cpp ContentNode.cpp FilePath.cpp ImgNode.cpp Node.cpp NodeBuilder.cpp ParNode.cpp SeqNode.cpp SmilEngine.cpp SmilTree.cpp SmilTreeBuilder.cpp Spine.cpp SpineBuilder.cpp TextNode.cpp TimeContainerNode.cpp
OBJS = AudioNode.o ContentNode.o FilePath.o ImgNode.o Node.o NodeBuilder.o ParNode.o SeqNode.o SmilEngine.o SmilTree.o SmilTreeBuilder.o Spine.o SpineBuilder.o TextNode.o TimeContainerNode.o
PROGRAM = SmilEngine

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS)

.PHONY: clean depend tags

clean:
	rm -f $(PROGRAM) $(OBJS)

depend:
	mv Makefile Makefile.bak
	sed '/^# DO NOT DELETE THIS LINE -- make depend depends on it.$$/q' < Makefile.bak > Makefile
	g++ -MM $(SRCS) >> Makefile

tags:
	ctags $(SRCS)

# DO NOT DELETE THIS LINE -- make depends on it.
AudioNode.o: AudioNode.cpp SmilEngineConstants.h SmilTime.h AudioNode.h SmilTree.h
ContentNode.o: ContentNode.cpp ContentNode.h
FilePath.o: FilePath.cpp FilePath.h
ImgNode.o: ImgNode.cpp SmilEngineConstants.h ImgNode.h SmilTree.h
Node.o: Node.cpp SmilEngineConstants.h TimeContainerNode.h Node.h AudioNode.h
SmilEngine.o: SmilEngine.cpp FilePath.h CommandParse.h SmilTreeBuilder.h SpineBuilder.h SmilTree.h Spine.h SmilEngine.h
