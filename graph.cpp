#include <graph.h>
#include <vertex.h>

Graph::Graph(QImage image)
{
	std::vector<std::vector<std::pair<int, Edge>>> adjList(vertices.size());

	for (auto& edge : edges) {
		adjList[edge.from].push_back(std::make_pair(edge.to, edge));
		adjList[edge.to].push_back(std::make_pair(edge.from, edge));
	}
	this->width = image.width();
	this->height = image.height();
	this->xObj = 0;
	this->yObj = 0;
	this->xBgd = 2;
	this->yBgd = 1;
	this->countN = 0;
	this->minIntensity = INT_MAX;
	this->maxIntensity = 0;
	int w = image.width();
	int h = image.height();
	this->vertices = std::vector<Vertex>(w * h + 2);
	this->vertices[w * h].id = w*h;
	this->vertices[w * h].intensity = -1;
	this->vertices[w * h].info = "S";
	this->vertices[w * h + 1].id = w*h+1;
	this->vertices[w * h + 1].intensity = -1;
	this->vertices[w * h + 1].info = "T";
	
	for (int y = 0; y < h; y++) {
		for (int x = 0; x < w; x++) {
			if (x == this->xObj && y == this->yObj) {
				this->vertices[this->xObj + this->yObj*w].info = "O";
			}
			if (x == this->xBgd && y == this->yBgd) {
				this->vertices[this->xBgd + this->yBgd * w].info = "B";
			}
			int i = y * w + x;
			this->vertices[i].id = i;
			QColor c = image.pixel(x, y);
			this->vertices[i].intensity = (static_cast<double>(c.red()) + c.green() + c.blue()) / 3;
			checkIntensities(this->vertices[i].intensity);
		}
	}

	for (int i = 0; i < w * h; i++) {
		int x = i % w;
		int y = i / w;
		if (x > 0) addEdge(i, i - 1);
		if (x < w - 1) addEdge(i, i + 1);
		if (y > 0) addEdge(i, i - w);
		if (y < h - 1) addEdge(i, i + w);
	}

	addEdgesSpecial(w, h); 
	for (int i = 0; i < edges.size(); i++) {
		edges[i].flow = 0;
	}
	qDebug() << "MAXIMALNY TOK JE " << this->maxFlow() << " :/";
}

Graph::Graph()
{
}

double Graph::getMinIntensity() {
	return this->minIntensity;
}

double Graph::getMaxIntensity() {
	return this->maxIntensity;
}

//S-Link -> OBJECT			kapacita = INF
//S-Link -> BACKGROUND		kapacita = 0
//T-Link -> OBJECT			kapacita = 0
//T-Link -> BACKGROUND		kapacita = INF
void Graph::addEdge(int from, int to)
{
	double capacity = 1.0;
	qDebug() << "from = " << from << " width*height = " << width * height << " to = " << to;
	if (from >= 0 && to >= 0 && from < width * height && to < width*height) {
		countN++;
		capacity = capacityNLink(from, to);
	}
	else if (from == (width * height) && (to == this->yObj * this->width + this->xObj)) {
		capacity = INT_MAX;
	}
	else if (from == (width * height) && (to == this->yBgd * this->width + this->xBgd)) {
		capacity = 0;
	}
	else if (from == (width * height) && (to != this->yObj * this->width + this->xObj) && (to != this->yBgd * this->width + this->xBgd)) {
		capacity = (this->maxIntensity - abs(vertices[this->yObj * this->width + this->xObj].intensity - vertices[to].intensity));
	}
	else if (to == (width * height + 1) && (from == this->yObj * this->width + this->xObj)) {
		capacity = 0;
	}
	else if (to == (width * height + 1) && (from == this->yBgd * this->width + this->xBgd)) {
		capacity = INT_MAX;
	}
	else {
		capacity = (this->maxIntensity - abs(vertices[this->yBgd * this->width + this->xBgd].intensity - vertices[from].intensity));
	}
	edges.push_back({from,to,capacity});
}

double Graph::capacityNLink(int from, int to) {
	return (maxIntensity - minIntensity) - abs(vertices[from].intensity - vertices[to].intensity);
}

void Graph::addEdgesSpecial(int width, int height) {
	for (int i = 0; i < width * height; i++) {
		addEdge(width * height, i);
	}
	for (int i = 0; i < width * height; i++) {
		addEdge(i, width * height + 1);
	}
}

void Graph::checkIntensities(double intensity) {
	if (intensity < minIntensity) {
		minIntensity = intensity;
	}
	if (intensity > maxIntensity) {
		maxIntensity = intensity;
	}
}

double Graph::maxFlow()
{
	int source = vertices.size() - 2;
	int sink = vertices.size() - 1;
	double maxFlow = 0;
	while (true) {
		std::vector<int> parent(vertices.size(), -1);
		std::vector<double> capacity(vertices.size(), 0);
		std::queue<int> q;
		q.push(source);
		parent[source] = source;
		capacity[source] = DBL_MAX;
		while (!q.empty()) {
			int u = q.front();
			q.pop();
			for (auto& edge : edges) {
				if (parent[edge.to] == -1 && edge.from == u && edge.capacity > edge.flow) {
					parent[edge.to] = u;
					capacity[edge.to] = std::min(capacity[u], edge.capacity - edge.flow);
					if (edge.to == sink) {
						break;
					}
					q.push(edge.to);
				}
			}
		}
		if (parent[sink] == -1) break;
		double pathFlow = capacity[sink];
		for (int v = sink; v != source; v = parent[v]) {
			int u = parent[v];
			for (auto& edge : edges) {
				if (edge.from == u && edge.to == v) {
					edge.flow += pathFlow;
					break;
				}
			}
			for (auto& edge : edges) {
				if (edge.from == v && edge.to == u) {
					edge.flow -= pathFlow;
					break;
				}
			}
		}
		maxFlow += pathFlow;
	}
	return maxFlow;
}

//double Graph::maxFlow()
//{
//	int source = vertices.size() - 2;
//	int sink = vertices.size() - 1;
//	double maxFlow = 0;
//	while (true) {
//		int* parent = new int[vertices.size()];
//		double* capacity = new double[vertices.size()];
//		memset(parent, -1, sizeof(int) * vertices.size());
//		memset(capacity, 0, sizeof(double) * vertices.size());
//		std::queue<int> q;
//		q.push(source);
//		parent[source] = source;
//		capacity[source] = DBL_MAX;
//		while (!q.empty()) {
//			int u = q.front();
//			q.pop();
//			for (auto& v : adjList[u]) {
//				if (parent[v] == -1 && edges[u][v].capacity > edges[u][v].flow) {
//					parent[v] = u;
//					capacity[v] = std::min(capacity[u], edges[u][v].capacity - edges[u][v].flow);
//					if (v == sink) {
//						break;
//					}
//					q.push(v);
//				}
//			}
//		}
//		if (parent[sink] == -1) {
//			delete[] parent;
//			delete[] capacity;
//			break;
//		}
//		double pathFlow = capacity[sink];
//		for (int v = sink; v != source; v = parent[v]) {
//			int u = parent[v];
//			edges[u][v].flow += pathFlow;
//			edges[v][u].flow -= pathFlow;
//		}
//		maxFlow += pathFlow;
//		delete[] parent;
//		delete[] capacity;
//	}
//	return maxFlow;
//}
