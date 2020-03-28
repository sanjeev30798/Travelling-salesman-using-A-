#include <bits/stdc++.h>
using namespace std;

typedef pair<int, int> iPair;

struct Graph
{
    int V, E;
    vector<pair<int, iPair>> edges;
    /* In a weighted graph, we need to store vertex */
    /* and weight pair for every edge */
    Graph(int V)
    {
        this->V = V;
    }
    void addEdge(int u, int v, int w)
    {
        edges.push_back({w, {u, v}});
    }

    int kruskalMST();
};

struct DisjointSets
{
    int *parent, *rnk;
    int n;

    DisjointSets(int n)
    {
        this->n = n;
        parent = new int[n + 1];
        rnk = new int[n + 1];

        for (int i = 0; i <= n; i++)
        {
            rnk[i] = 0;
            parent[i] = i;
        }
    }

    int find(int u)
    {
        if (u != parent[u])
            parent[u] = find(parent[u]);
        return parent[u];
    }

    void merge(int x, int y)
    {
        x = find(x), y = find(y);
        if (rnk[x] > rnk[y])
            parent[y] = x;
        else
            parent[x] = y;
        if (rnk[x] == rnk[y])
            rnk[y]++;
    }
};

/* Functions returns weight of the MST*/
int Graph::kruskalMST()
{
    int mst_wt = 0;
    sort(edges.begin(), edges.end());

    DisjointSets ds(V);

    vector<pair<int, iPair>>::iterator it;
    for (it = edges.begin(); it != edges.end(); it++)
    {
        int u = it->second.first;
        int v = it->second.second;

        int set_u = ds.find(u);
        int set_v = ds.find(v);
        if (set_u != set_v)
        {
            mst_wt += it->first;
            ds.merge(set_u, set_v);
        }
    }

    return mst_wt;
}

int findPathCost(vector<pair<int, int>> adj[], int u, int v)
{
    for (auto it = adj[u].begin(); it != adj[u].end(); ++it)
    {
        if ((*it).first == v)
        {
            return (*it).second;
        }
    }
    return -1;
}

int isExistOpen(vector<pair<int, iPair>> open, int v)
{
    for (auto it = open.begin(); it != open.end(); ++it)
    {
        if ((*it).second.second == v)
        {
            return (*it).first;
        }
    }
    return -1;
}

void findAndUpdate(vector<pair<int, iPair>> &open, int tc, int pc, int v)
{
    for (auto it = open.begin(); it != open.end(); ++it)
    {
        if ((*it).second.second == v)
        {
            (*it).second.first = pc;
            (*it).first = tc;
            return;
        }
    }
}
// distance to the nearest unvisited city from the current city
int findMinh1(int *adj[], int v, vector<int> remainEle)
{
    int mini = remainEle.size() > 1 ? INT_MAX : 0;
    for (auto it = remainEle.begin(); it != remainEle.end(); ++it)
    {
        if (adj[v][*it] < mini && *it != v)
        {
            mini = adj[v][*it];
        }
    }
    return mini;
}
// nearest distance from an unvisited city to the start city
int findMinh2(int *adj[], int v, vector<int> remainEle)
{
    int mini = remainEle.size() > 1 ? INT_MAX : 0;
    for (auto it = remainEle.begin(); it != remainEle.end(); ++it)
    {
        if (adj[0][*it] < mini && *it != v)
        {
            mini = adj[0][*it];
        }
    }
    return mini;
}

int heuristicCost(int v, int *adj[], vector<int> remainEle)
{
    int h1, h2, h3;
    // distance to the nearest unvisited city from the current city
    h1 = findMinh1(adj, v, remainEle);
    // nearest distance from an unvisited city to the start city
    h3 = findMinh2(adj, v, remainEle);

    // estimated distance to travel all the unvisited cities
    // (MST heuristic used here)
    // Create a graph and add all edges below current node
    Graph graph(remainEle.size() - 1);
    int i = 0, j = 0;
    for (auto it = remainEle.begin(); it != remainEle.end(); ++it)
    {
        if (*it != v)
        {
            j = i + 1;
            for (auto jt = it + 1; jt != remainEle.end(); ++jt)
            {
                if (*jt != v)
                {
                    graph.addEdge(i, j, adj[*it][*jt]);
                    ++j;
                }
            }
            ++i;
        }
    }
    h2 = graph.kruskalMST();
    return h1 + h2 + h3;
}

int AStar(int **adj, int V)
{
    //open list of vertices
    vector<pair<int, iPair>> open;
    //close list of verices
    vector<int> closed;
    //left vertices
    vector<int> remainEle;
    for (int i = 0; i < V; ++i)
        remainEle.push_back(i);
    //heuristic for first node
    int hc = heuristicCost(0, adj, remainEle);
    int pc = 0;
    int tc = hc + pc;
    int minPathCost = 0;
    open.push_back({tc, {pc, 0}});
    while (!remainEle.empty())
    {
        // finding min cost pair in open to expand
        auto mini = std::min_element(open.cbegin(), open.cend(), [](const auto &lhs, const auto &rhs) {
            return lhs.first < rhs.first;
        });
        //storing the extracted pair
        pair<int, iPair> storePair = *mini;
        //removing it from open
        open.erase(mini);

        int ppc = storePair.second.first;
        int city = storePair.second.second;
        closed.push_back(city);
        //removinf from left items
        remainEle.erase(find(remainEle.begin(), remainEle.end(), city));
        if (remainEle.empty())
        {
            minPathCost = ppc + adj[0][city];
        }

        for (auto it = remainEle.begin(); it != remainEle.end(); ++it)
        {
            hc = heuristicCost((*it), adj, remainEle);
            pc = ppc + adj[city][*it];
            tc = pc + hc;
            int existTCO = isExistOpen(open, (*it));
            
            if (existTCO != -1)
            {
                findAndUpdate(open, tc, pc, (*it));
            }
            else
            {
                open.push_back({tc, {pc, (*it)}});
            }
        }
    }
    return minPathCost;
}
/* Driver program to test methods of graph class */
int main()
{
    int u, v, w, V, E;
    // taking no of vertices
    // and cost of path between each vertex
    cout << "Enter the no. of cities: ";
    cin >> V;
    int **adj;
    adj = new int *[V];
    for (int i = 0; i < V; i++)
        adj[i] = new int[V];
    cout << "Cost in adjacency matrix format : " << endl;
    for (int i = 0; i < V; ++i)
    {
        for (int j = 0; j < V; ++j)
            cin >> adj[i][j];
    }
    int optimal_Cost = AStar(adj, V);
    cout << "Optimal Cost : " << optimal_Cost << '\n';
    return 0;
}
