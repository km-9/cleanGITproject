public void dijkstra(String sourceName){
  clearData();
  Vertex source = getVertex(sourceName);
  source.dist = 0;
  pq.add(source);
  while(!pq.isEmpty()){
    Vertex next = pq.poll();
    if(next.visisted)
      continue;
    next.visited = true;
    result.add(next);
    for(Edge edge : next.adj){
      if(next.dist + edge.cost < edge.dest.dist){
        edge.dest.deist = next.dist + edge.cost;
        edge.dest.prev = next;
        pq.add(edge.dest);
      }
    }
  }
}
//*****************************************************************************
//#4
public void dijkstra(String sourceName){
  clearData();
  int pathCount = 0;
  Vertex source = getVertex(sourceName);
  source.dist = 0;
  pq.add(source);
  while(!pq.isEmpty()){
    Vertex next = pq.poll();
    if(next.visisted)
      continue;
    next.visited = true;
    result.add(next);
    for(Edge edge : next.adj){
      if(next.dist + edge.cost < edge.dest.dist){
        edge.dest.dist = next.dist + edge.cost;
        edge.dest.prev = next;
        pq.add(edge.dest);
        pathCount = 1;
      }
      if(next.dist + edge.cost == edge.dest.dist)
        pathCount++;
    }
  }
}

/*
If a directed graph G = (V, E) is acyclic and no edge between u and v is in E, then
at least one of (u, v) or (v, u) can be added to E without creating a cycle.





*/
