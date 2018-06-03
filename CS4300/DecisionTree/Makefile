CC = g++
DEBUG = -g
CFLAGS = -c
OBJS = DataPrep.o TreeBuilder.o DecisionTree.o TreeNode.o

DecisionTree : $(OBJS)
	$(CC) $(OBJS) main.cpp -o DecisionTree

DataPrep.o : DataPrep.h DataPrep.cpp
	$(CC) $(CFLAGS) DataPrep.cpp

TreeBuilder.o : TreeBuilder.h TreeBuilder.cpp
	$(CC) $(CFLAGS) TreeBuilder.cpp

DecisionTree.o : DecisionTree.h DecisionTree.cpp
	$(CC) $(CFLAGS) DecisionTree.cpp

TreeNode.o : TreeNode.h TreeNode.cpp
	$(CC) $(CFLAGS) TreeNode.cpp

clean :
	rm *.o DecisionTree
