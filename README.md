# Segmentation

This project for my Bachelor Thesis uses the Ford-Fulkerson network flow algorithm to solve image segmentation problem. To find paths from the source node to the sink node in the Ford-Fullkerson algorithm the breadth first search (BFS) was used. Each vertex is connected to adjacent vertices by edges. The capacity of the edges was determined by the difference in the pixels' intesities. Before the segmentation, user has to choose seed pixels for the object and the background.

Our goal for this project was to separate the background from the object.
