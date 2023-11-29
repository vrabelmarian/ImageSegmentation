#pragma once

#include <QImage>
#include <QPixelFormat>
#include <vector>
#include <qdebug.h>
#include <queue>

class Graph {
public:
	struct Edge {
		int from, to;
		double capacity;
		double flow;
	};
	int countN;
	int xObj;
	int yObj;
	int xBgd;
	int yBgd;
	int width;
	int height;
private:
	struct Vertex {
		int id;
		double intensity;
		const char* info;
	};
	double minIntensity;
	double maxIntensity;

	std::vector<Vertex> vertices;
	std::vector<Edge> edges;

public:
	Graph(QImage image);
	Graph();
	void addEdge(int from, int to);
	void addEdgesSpecial(int width, int height);
	void checkIntensities(double intensity);
	double getMinIntensity();
	double getMaxIntensity();
	double capacityNLink(int from, int to);
	double maxFlow();
};
