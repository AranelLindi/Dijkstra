#include "_graphml/graphml.h"

#include <cstdio>

#define plot(x) std::cout << x << std::endl;

int main(void) {


    plot("Node:")
    plot(sizeof(Node));
    plot(sizeof(Node*));
    plot("Edge:");
    plot(sizeof(Edge));
    plot(sizeof(Edge*));
    plot("Key:");
    plot(sizeof(Key));
    plot(sizeof(Key*));
    plot("Data:");
    plot(sizeof(Data));
    plot(sizeof(Data*));
    plot("GraphObject:")
    plot(sizeof(GraphObject));
    plot(sizeof(GraphObject*));
    plot("EdgeTyp:");
    plot(sizeof(EdgeType));
    plot(sizeof(EdgeType*));

    return 0;
}