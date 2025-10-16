#ifndef __FRAMEWORK_GRAPH_H__
#define __FRAMEWORK_GRAPH_H__

#include "../FWLocal.h"
#include "../FWTypeDefine.h"

FW_NS_BEGIN


namespace FWGraph {

	typedef int Weight;
	const Weight WEIGHT_MAX = INT_MAX;

	class EdgeInterface {
	public:
		virtual Weight getWeight() const = 0;
	};

	/*
	* �ڲ�ʵ����ʮ����������
	* ��֧���Ի���
	* ��֧��ƽ�б�
	*/
	template<typename V, typename E>
	class GraphDirected {
	public:
		/*
		* ��������ص�
		* ���� false �������������� true ����ֹͣ����
		* �����ڴ˻ص��жԶ����߽�����ɾ����
		*/
		typedef std::function<bool(size_t, V&)> TraverseCallback;

		struct EdgeKey {
			size_t from;
			size_t to;
			bool operator<(const EdgeKey& other) const
			{
				if (from < other.from)
					return true;
				if (other.from < from)
					return false;
				return to < other.to;
			}
			bool operator==(const EdgeKey& other) const
			{
				return from == other.from && to == other.to;
			}
		};

		template<typename E>
		struct Edge {
			EdgeKey	edgeKey;
			/*
			* Ȩֵ
			* ������ʵ�� EdgeInterface �ӿڵ����ͻ� Weight ����
			*/
			E		data;
			Weight getWeight() const
			{
				return _getWeight<E>();
			}
		private:
			template <typename E>
			Weight _getWeight() const
			{
				return data.getWeight();
			}
			template <>
			Weight _getWeight<Weight>() const
			{
				return data;
			}
		};

		template<typename V>
		struct Vertex {
			std::set<size_t>	edge_in;
			std::set<size_t>	edge_out;
			/*
			* ��������
			* ���ظ����� insertEdgeWithVertexData��getVertexIndex ����V����Ϊ�����Ľӿ��ڲ�ֻ��ƥ��һ��
			* ���鲻�ظ�������һ����ɾ����֮������޷�ȷ��˭��˭
			*/
			V					data;
		};

		GraphDirected(size_t vertexCapacity = 256, size_t edgeCapacity = 256);
		GraphDirected(const GraphDirected& other);
		GraphDirected(GraphDirected&& right);
		~GraphDirected();

		GraphDirected& operator=(const GraphDirected& other);
		GraphDirected& operator=(GraphDirected&& right);

		/*
		* �����²��붥����±�
		*/
		size_t insertVertex(const V& data);

		/*
		* ɾ�����������ж���ͱߵ��±�λ��
		* ����ֵ�����ر�ɾ���������һ�������±꣨���򣩣����������û����һ�����㷵�� npos
		*/
		size_t removeVertex(size_t index);

		/*
		* �����²���ߵ��±꣬����ʧ�ܷ��� npos
		*/
		size_t insertEdge(size_t from, size_t to, const E& data);

		/*
		* �����²���ߵ��±꣬����ʧ�ܷ��� npos
		*/
		size_t insertEdgeWithVertexData(const V& fromData, const V& toData, const E& data);

		/*
		* ɾ�����������бߺͶ�����±�λ��
		*/
		void removeEdge(size_t from, size_t to);

		/*
		* ɾ�����������бߺͶ�����±�λ��
		* ����ֵ�����ر�ɾ���ߵ���һ�����±꣨���򣩣����������û����һ���߷��� npos
		*/
		size_t removeEdge(size_t index);

		/*
		* ����ƥ�䵽�ĵ�һ��������±꣬ƥ�䲻������ npos
		*/
		size_t getVertexIndex(const V& data) const;

		/*
		* �Ҳ������㽫���׳� std::bad_exception �쳣
		*/
		V& getVertexData(size_t index);

		/*
		* �Ҳ����߽����׳� std::bad_exception �쳣
		*/
		E& getEdgeData(size_t from, size_t to);

		/*
		* �Ҳ����߽����׳� std::bad_exception �쳣
		*/
		E& getEdgeData(size_t index);

		bool isExistVertex(size_t index) const;

		bool isExistEdge(size_t from, size_t to) const;

		bool isExistEdge(size_t index) const;

		size_t inDegree(size_t index) const;

		size_t outDegree(size_t index) const;

		void vertexDFS(size_t index, const TraverseCallback& callback);

		void vertexBFS(size_t index, const TraverseCallback& callback);

		void vertexDFS(const TraverseCallback& callback);

		void vertexBFS(const TraverseCallback& callback);

		/*
		* �ɹ����� true ���л����� false
		*/
		bool topologicalSort(std::vector<size_t>& indexes_out) const;

		/*
		* �ɹ����� true ���������󷵻� false �����ָ�Ȩ��С����Ȩ·����������� false
		*/
		bool ShortestPathsSPFA(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const;

		/*
		* �ɹ����� true ���������󷵻� false ������и�Ȩ�߽���õ�����Ľ������Ȩ��ͬ��Ҳ��õ�����Ľ��
		*/
		bool ShortestPathsDijkstra(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const;

		/*
		* ���ָ�Ȩ��С����Ȩ·�����������õ�����Ľ��
		*/
		void ShortestPathsFloyd(std::vector<std::vector<Weight> >& weights_out, std::vector<std::vector<size_t> >& paths_out) const;

		std::vector<std::vector<Weight> > makeAdjacencyMatrix() const;

		size_t vertexSize() const;

		size_t edgeSize() const;

		void clear();

		static const size_t npos = static_cast<size_t>(-1);

		/*
		* ����ֵ���������
		*/
		static std::list<size_t> makePathWithResultForDijkstraAndSPFA(size_t endIndex, std::vector<Weight>& weights, std::vector<size_t>& paths);

		/*
		* ����ֵ���������
		*/
		static std::list<size_t> makePathWithResultForFloyd(size_t beginIndex, size_t endIndex, std::vector<std::vector<Weight> >& weights, std::vector<std::vector<size_t> >& paths);

	private:
		void _changeEdgeLink(const std::set<size_t>& edgeSet, bool changeFrom, size_t newIndex);
		void _changeEdgeAndVertexLink(size_t vertexIndex, bool isIn, size_t originIndex, size_t newIndex);

	private:
		std::vector<Vertex<V> >		m_Vertexes;
		std::vector<Edge<E> >		m_Edges;
		std::map<EdgeKey, size_t>	m_EdgeKeys;
	};

	template<typename V, typename E>
	GraphDirected<V, E>::GraphDirected(size_t vertexCapacity /*= 256*/, size_t edgeCapacity /*= 256*/)
	{
		m_Vertexes.reserve(vertexCapacity);
		m_Edges.reserve(edgeCapacity);
	}

	template<typename V, typename E>
	GraphDirected<V, E>::GraphDirected(const GraphDirected& other)
	{
		m_Vertexes = other.m_Vertexes;
		m_Edges = other.m_Edges;
		m_EdgeKeys = other.m_EdgeKeys;
	}

	template<typename V, typename E>
	GraphDirected<V, E>::GraphDirected(GraphDirected&& right)
	{
		m_Vertexes = std::move(right.m_Vertexes);
		m_Edges = std::move(right.m_Edges);
		m_EdgeKeys = std::move(right.m_EdgeKeys);
	}

	template<typename V, typename E>
	GraphDirected<V, E>::~GraphDirected()
	{
		clear();
	}

	template<typename V, typename E>
	GraphDirected<V, E>& GraphDirected<V, E>::operator=(const GraphDirected& other)
	{
		m_Vertexes = other.m_Vertexes;
		m_Edges = other.m_Edges;
		m_EdgeKeys = other.m_EdgeKeys;
		return *this;
	}

	template<typename V, typename E>
	GraphDirected<V, E>& GraphDirected<V, E>::operator=(GraphDirected&& right)
	{
		m_Vertexes = std::move(right.m_Vertexes);
		m_Edges = std::move(right.m_Edges);
		m_EdgeKeys = std::move(right.m_EdgeKeys);
		return *this;
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::insertVertex(const V& data)
	{
		Vertex<V> vertex;
		vertex.data = data;
		m_Vertexes.push_back(std::move(vertex));
		return m_Vertexes.size() - 1;
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::removeVertex(size_t index)
	{
		size_t next = npos;
		if (!isExistVertex(index))
			return next;
		Vertex<V>& vertex = m_Vertexes[index];
		while (vertex.edge_in.size() > 0)
			removeEdge(*vertex.edge_in.begin());
		while (vertex.edge_out.size() > 0)
			removeEdge(*vertex.edge_out.begin());
		if (m_Vertexes.size() > 1)
		{
			if (index != m_Vertexes.size() - 1)
			{
				next = index;
				m_Vertexes[index] = std::move(m_Vertexes[m_Vertexes.size() - 1]);
				_changeEdgeLink(vertex.edge_in, false, index);
				_changeEdgeLink(vertex.edge_out, true, index);
			}
			m_Vertexes.pop_back();
		}
		else
			m_Vertexes.clear();
		return next;
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::insertEdge(size_t from, size_t to, const E& data)
	{
		if (!isExistVertex(from) || !isExistVertex(to) || from == to)
			return npos;
		EdgeKey edgeKey = { from, to };
		if (m_EdgeKeys.find(edgeKey) != m_EdgeKeys.end())
			return npos;
		Edge<E> edge;
		edge.data = data;
		edge.edgeKey = edgeKey;
		m_Edges.push_back(std::move(edge));
		size_t edgeIndex = m_Edges.size() - 1;
		m_EdgeKeys[edgeKey] = edgeIndex;
		m_Vertexes[from].edge_out.insert(edgeIndex);
		m_Vertexes[to].edge_in.insert(edgeIndex);
		return edgeIndex;
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::insertEdgeWithVertexData(const V& fromData, const V& toData, const E& data)
	{
		size_t from = npos;
		size_t to = npos;
		for (size_t i = 0; i < m_Vertexes.size() && (from == npos || to == npos); i++)
		{
			if (from == npos && m_Vertexes[i].data == fromData)
				from = i;
			else if (to == npos && m_Vertexes[i].data == toData)
				to = i;
		}
		if (from == npos || to == npos)
			return npos;
		return insertEdge(from, to, data);
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::removeEdge(size_t from, size_t to)
	{
		EdgeKey edgeKey = { from, to };
		std::map<EdgeKey, size_t>::iterator it = m_EdgeKeys.find(edgeKey);
		if (it == m_EdgeKeys.end())
			return;
		removeEdge(it->second);
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::removeEdge(size_t index)
	{
		size_t next = npos;
		if (!isExistEdge(index))
			return next;
		Edge<E>& edge = m_Edges[index];
		m_Vertexes[edge.edgeKey.from].edge_out.erase(index);
		m_Vertexes[edge.edgeKey.to].edge_in.erase(index);
		if (m_Edges.size() > 1)
		{
			m_EdgeKeys.erase(edge.edgeKey);
			if (index != m_Edges.size() - 1)
			{
				next = index;
				m_EdgeKeys[m_Edges[m_Edges.size() - 1].edgeKey] = index;
				m_Edges[index] = std::move(m_Edges[m_Edges.size() - 1]);
				_changeEdgeAndVertexLink(m_Edges[index].edgeKey.from, false, m_Edges.size() - 1, index);
				_changeEdgeAndVertexLink(m_Edges[index].edgeKey.to, true, m_Edges.size() - 1, index);
			}
			m_Edges.pop_back();
		}
		else
		{
			m_Edges.clear();
			m_EdgeKeys.clear();
		}
		return next;
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::getVertexIndex(const V& data) const
	{
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			if (m_Vertexes[i].data == data)
				return i;
		return npos;
	}

	template<typename V, typename E>
	V& GraphDirected<V, E>::getVertexData(size_t index)
	{
		if (!isExistVertex(index))
			throw std::bad_exception();
		return m_Vertexes[index].data;
	}

	template<typename V, typename E>
	E& GraphDirected<V, E>::getEdgeData(size_t from, size_t to)
	{
		EdgeKey edgeKey = { from, to };
		std::map<EdgeKey, size_t>::iterator it = m_EdgeKeys.find(edgeKey);
		if (it == m_EdgeKeys.end())
			throw std::bad_exception();
		return getEdgeData(it->second);
	}

	template<typename V, typename E>
	E& GraphDirected<V, E>::getEdgeData(size_t index)
	{
		if (!isExistEdge(index))
			throw std::bad_exception();
		return m_Edges[index].data;
	}

	template<typename V, typename E>
	bool GraphDirected<V, E>::isExistVertex(size_t index) const
	{
		return index < m_Vertexes.size();
	}

	template<typename V, typename E>
	bool GraphDirected<V, E>::isExistEdge(size_t from, size_t to) const
	{
		EdgeKey edgeKey = { from, to };
		return m_EdgeKeys.find(edgeKey) != m_EdgeKeys.end();
	}

	template<typename V, typename E>
	bool GraphDirected<V, E>::isExistEdge(size_t index) const
	{
		return index < m_Edges.size();
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::inDegree(size_t index) const
	{
		if (!isExistVertex(index))
			return 0;
		return m_Vertexes[index].edge_in.size();
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::outDegree(size_t index) const
	{
		if (!isExistVertex(index))
			return 0;
		return m_Vertexes[index].edge_out.size();
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::vertexDFS(size_t index, const TraverseCallback& callback)
	{
		if (!isExistVertex(index))
			return;
		std::set<size_t> already;
		std::stack<size_t> children;
		children.push(index);
		while (children.size() > 0)
		{
			size_t idx = children.top();
			children.pop();
			std::pair<std::set<size_t>::iterator, bool> r = already.insert(idx);
			if (!r.second)
				continue;
			Vertex<V>& vertex = m_Vertexes[idx];
			if (callback(idx, vertex.data))
				break;
			for (std::set<size_t>::reverse_iterator it = vertex.edge_out.rbegin(); it != vertex.edge_out.rend(); it++)
				children.push(m_Edges[*it].edgeKey.to);
		}
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::vertexBFS(size_t index, const TraverseCallback& callback)
	{
		if (!isExistVertex(index))
			return;
		std::set<size_t> already;
		std::queue<size_t> children;
		children.push(index);
		while (children.size() > 0)
		{
			size_t idx = children.front();
			children.pop();
			std::pair<std::set<size_t>::iterator, bool> r = already.insert(idx);
			if (!r.second)
				continue;
			Vertex<V>& vertex = m_Vertexes[idx];
			if (callback(idx, vertex.data))
				break;
			for (std::set<size_t>::iterator it = vertex.edge_out.begin(); it != vertex.edge_out.end(); it++)
				children.push(m_Edges[*it].edgeKey.to);
		}
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::vertexDFS(const TraverseCallback& callback)
	{
		if (m_Vertexes.size() == 0)
			return;
		std::set<size_t> vertexIndexes;
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			vertexIndexes.insert(i);
		TraverseCallback cb = [&callback, &vertexIndexes](size_t index, V& data)->bool {
			if (vertexIndexes.erase(index) == 0)
				return false;
			return callback(index, data);
		};
		while (vertexIndexes.size() > 0)
			vertexDFS(*vertexIndexes.begin(), cb);
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::vertexBFS(const TraverseCallback& callback)
	{
		if (m_Vertexes.size() == 0)
			return;
		std::set<size_t> vertexIndexes;
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			vertexIndexes.insert(i);
		TraverseCallback cb = [&callback, &vertexIndexes](size_t index, V& data)->bool {
			if (vertexIndexes.erase(index) == 0)
				return false;
			return callback(index, data);
		};
		while (vertexIndexes.size() > 0)
			vertexBFS(*vertexIndexes.begin(), cb);
	}

	template<typename V, typename E>
	bool GraphDirected<V, E>::topologicalSort(std::vector<size_t>& indexes_out) const
	{
		indexes_out.clear();
		if (m_Vertexes.size() == 0)
			return true;
		indexes_out.reserve(m_Vertexes.size());
		std::vector<size_t> inDegrees;
		std::queue<size_t> zeroDegrees;
		for (size_t i = 0; i < m_Vertexes.size(); i++)
		{
			size_t d = inDegree(i);
			inDegrees.push_back(d);
			if (d == 0)
				zeroDegrees.push(i);
		}
		while (zeroDegrees.size() > 0)
		{
			size_t idx = zeroDegrees.front();
			zeroDegrees.pop();
			indexes_out.push_back(idx);
			const Vertex<V>& vertex = m_Vertexes[idx];
			for (std::set<size_t>::const_iterator it = vertex.edge_out.cbegin(); it != vertex.edge_out.cend(); it++)
			{
				if (--inDegrees[m_Edges[*it].edgeKey.to] == 0)
					zeroDegrees.push(m_Edges[*it].edgeKey.to);
			}
		}
		bool r = indexes_out.size() == m_Vertexes.size();
		if (!r)
			indexes_out.clear();
		return r;
	}

	template<typename V, typename E>
	bool GraphDirected<V, E>::ShortestPathsSPFA(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const
	{
		weights_out.clear();
		paths_out.clear();
		if (!isExistVertex(index))
			return false;
		weights_out.resize(m_Vertexes.size(), WEIGHT_MAX);
		paths_out.resize(m_Vertexes.size(), npos);
		struct OutInfo {
			size_t from;
			size_t index;
			Weight weight;
		};
		std::queue<OutInfo> children;
		weights_out[index] = 0;
		paths_out[index] = index;
		for (std::set<size_t>::const_iterator it = m_Vertexes[index].edge_out.cbegin(); it != m_Vertexes[index].edge_out.cend(); it++)
			children.push({ m_Edges[*it].edgeKey.from, m_Edges[*it].edgeKey.to, m_Edges[*it].getWeight() });
		std::vector<size_t> times;
		times.resize(m_Vertexes.size(), 0);
		while (children.size() > 0)
		{
			OutInfo outInfo = children.front();
			children.pop();
			if (weights_out[outInfo.from] + outInfo.weight < weights_out[outInfo.index])
			{
				const Vertex<V>& vertex = m_Vertexes[outInfo.index];
				if (++times[outInfo.index] == m_Vertexes.size())
					return false; // ��Ȩ��С����Ȩ·���γ���ѭ��
				weights_out[outInfo.index] = weights_out[outInfo.from] + outInfo.weight;
				paths_out[outInfo.index] = outInfo.from;
				for (std::set<size_t>::const_iterator it = vertex.edge_out.cbegin(); it != vertex.edge_out.cend(); it++)
					children.push({ m_Edges[*it].edgeKey.from, m_Edges[*it].edgeKey.to, m_Edges[*it].getWeight() });
			}
		}
		return true;
	}

	template<typename V, typename E>
	bool GraphDirected<V, E>::ShortestPathsDijkstra(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const
	{
		weights_out.clear();
		paths_out.clear();
		if (!isExistVertex(index))
			return false;
		weights_out.resize(m_Vertexes.size(), WEIGHT_MAX);
		paths_out.resize(m_Vertexes.size(), npos);
		std::vector<bool> checked(m_Vertexes.size(), false);
		weights_out[index] = 0;
		paths_out[index] = index;
		checked[index] = true;
		struct OutInfo {
			size_t from;
			size_t index;
			Weight weight;
			bool operator<(const OutInfo& other) const
			{
				return weight > other.weight; //�����õ����ѣ��������ﷴ���ж�
			}
		};
		std::priority_queue<OutInfo> heap_vertexes;
		for (std::set<size_t>::const_iterator it = m_Vertexes[index].edge_out.cbegin(); it != m_Vertexes[index].edge_out.cend(); it++)
			heap_vertexes.push({ index, m_Edges[*it].edgeKey.to, m_Edges[*it].getWeight() });
		while (heap_vertexes.size() > 0)
		{
			OutInfo outInfo =  heap_vertexes.top();
			heap_vertexes.pop();
			if (checked[outInfo.index])
				continue;
			checked[outInfo.index] = true;
			if (outInfo.weight < weights_out[outInfo.index])
			{
				weights_out[outInfo.index] = outInfo.weight;
				paths_out[outInfo.index] = outInfo.from;
				const Vertex<V>& vertex = m_Vertexes[outInfo.index];
				for (std::set<size_t>::const_iterator it = vertex.edge_out.cbegin(); it != vertex.edge_out.cend(); it++)
					if (!checked[m_Edges[*it].edgeKey.to])
						heap_vertexes.push({ outInfo.index, m_Edges[*it].edgeKey.to, weights_out[outInfo.index] + m_Edges[*it].getWeight() });
			}
		}
		return true;
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::ShortestPathsFloyd(std::vector<std::vector<Weight> >& weights_out, std::vector<std::vector<size_t> >& paths_out) const
	{
		weights_out.clear();
		paths_out.clear();
		if (m_Vertexes.size() == 0)
			return;
		paths_out.resize(m_Vertexes.size(), std::vector<size_t>(m_Vertexes.size(), npos));
		std::vector<std::vector<Weight> > adjMatrix = makeAdjacencyMatrix();
		for (size_t k = 0; k < m_Vertexes.size(); k++)
			for (size_t i = 0; i < m_Vertexes.size(); i++)
				for (size_t j = 0; j < m_Vertexes.size(); j++)
					if (adjMatrix[i][k] != WEIGHT_MAX && adjMatrix[k][j] != WEIGHT_MAX && adjMatrix[i][j] > adjMatrix[i][k] + adjMatrix[k][j])
					{
						adjMatrix[i][j] = adjMatrix[i][k] + adjMatrix[k][j];
						paths_out[i][j] = k;
					}
		weights_out = std::move(adjMatrix);
	}

	template<typename V, typename E>
	std::vector<std::vector<Weight> > GraphDirected<V, E>::makeAdjacencyMatrix() const
	{
		std::vector<std::vector<Weight> > adjMatrix;
		adjMatrix.resize(m_Vertexes.size(), std::vector<Weight>(m_Vertexes.size(), WEIGHT_MAX));
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			adjMatrix[i][i] = 0;
		for (typename std::vector<Edge<E> >::const_iterator it = m_Edges.cbegin(); it != m_Edges.cend(); it++)
			adjMatrix[it->edgeKey.from][it->edgeKey.to] = it->getWeight();
		return std::move(adjMatrix);
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::vertexSize() const
	{
		return m_Vertexes.size();
	}

	template<typename V, typename E>
	size_t GraphDirected<V, E>::edgeSize() const
	{
		return m_Edges.size();
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::clear()
	{
		m_Vertexes.clear();
		m_Edges.clear();
		m_EdgeKeys.clear();
	}

	template<typename V, typename E>
	std::list<size_t> GraphDirected<V, E>::makePathWithResultForDijkstraAndSPFA(size_t endIndex, std::vector<Weight>& weights, std::vector<size_t>& paths)
	{
		std::list<size_t> path;
		if (endIndex >= weights.size() || weights[endIndex] == WEIGHT_MAX)
			return path;
		size_t index = endIndex;
		do {
			path.push_front(index);
			index = paths[index];
			if (index == paths[index])
				break;
		} while (true);
		return path;
	}

	template<typename V, typename E>
	std::list<size_t> GraphDirected<V, E>::makePathWithResultForFloyd(size_t beginIndex, size_t endIndex, std::vector<std::vector<Weight> >& weights, std::vector<std::vector<size_t> >& paths)
	{
		std::list<size_t> path;
		if (beginIndex >= weights.size() || endIndex >= weights[beginIndex].size() || weights[beginIndex][endIndex] == WEIGHT_MAX)
			return path;
		struct OpInfo {
			size_t from;
			size_t to;
			size_t jump;
		};
		std::stack<OpInfo> opStack;
		opStack.push({ beginIndex, endIndex, paths[beginIndex][endIndex] });
		while (opStack.size() > 0)
		{
			OpInfo opInfo = opStack.top();
			opStack.pop();
			if (opInfo.jump == npos)
			{
				path.push_back(opInfo.to);
				continue;
			}
			opStack.push({ opInfo.jump, opInfo.to, paths[opInfo.jump][opInfo.to] });
			opStack.push({ opInfo.from, opInfo.jump, paths[opInfo.from][opInfo.jump] });
		}
		return path;
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::_changeEdgeLink(const std::set<size_t>& edgeSet, bool changeFrom, size_t newIndex)
	{
		for (std::set<size_t>::const_iterator it = edgeSet.cbegin(); it != edgeSet.cend(); it++)
		{
			Edge<E>& edge = m_Edges[*it];
			m_EdgeKeys.erase(edge.edgeKey);
			if (changeFrom)
				edge.edgeKey.from = newIndex;
			else
				edge.edgeKey.to = newIndex;
			m_EdgeKeys[edge.edgeKey] = *it;
		}
	}

	template<typename V, typename E>
	void GraphDirected<V, E>::_changeEdgeAndVertexLink(size_t vertexIndex, bool isIn, size_t originIndex, size_t newIndex)
	{
		Vertex<V>& vertex = m_Vertexes[vertexIndex];
		std::set<size_t>& edgeSet = isIn ? vertex.edge_in : vertex.edge_out;
		edgeSet.erase(originIndex);
		edgeSet.insert(newIndex);
	}


	////////////////////////////////////////////////////////////////////////////////////////////////////


	/*
	* �ڲ�ʵ�����ڽ���������
	* ��֧���Ի���
	* ��֧��ƽ�б�
	*/
	template<typename V, typename E>
	class GraphUndirected {
	public:
		/*
		* ��������ص�
		* ���� false �������������� true ����ֹͣ����
		* �����ڴ˻ص��жԶ����߽�����ɾ����
		*/
		typedef std::function<bool(size_t, V&)> TraverseCallback;

		struct EdgeKey {
			size_t v1;
			size_t v2;
			bool operator<(const EdgeKey& other) const
			{
				if (v1 == other.v1)
					return v2 < other.v2;
				//if (v2 == other.v2)
				//	return v1 < other.v1;
				if (v1 == other.v2)
					return v2 < other.v1;
				if (v2 == other.v1)
					return v1 < other.v2;
				return v1 < other.v1;
			}
			bool operator==(const EdgeKey& other) const
			{
				return (v1 == other.v1 && v2 == other.v2) || (v1 == other.v2 && v2 == other.v1);
			}
		};

		template<typename E>
		struct Edge {
			EdgeKey	edgeKey;
			/*
			* Ȩֵ
			* ������ʵ�� EdgeInterface �ӿڵ����ͻ� Weight ����
			*/
			E		data;
			Weight getWeight() const
			{
				return _getWeight<E>();
			}
		private:
			template <typename E>
			Weight _getWeight() const
			{
				return data.getWeight();
			}
			template <>
			Weight _getWeight<Weight>() const
			{
				return data;
			}
		};

		template<typename V>
		struct Vertex {
			std::set<size_t>	edges;
			/*
			* ��������
			* ���ظ����� insertEdgeWithVertexData��getVertexIndex ����V����Ϊ�����Ľӿ��ڲ�ֻ��ƥ��һ��
			* ���鲻�ظ�������һ����ɾ����֮������޷�ȷ��˭��˭
			*/
			V					data;
		};

		GraphUndirected(size_t vertexCapacity = 256, size_t edgeCapacity = 256);
		GraphUndirected(const GraphUndirected& other);
		GraphUndirected(GraphUndirected&& right);
		~GraphUndirected();

		GraphUndirected& operator=(const GraphUndirected& other);
		GraphUndirected& operator=(GraphUndirected&& right);

		/*
		* �����²��붥����±�
		*/
		size_t insertVertex(const V& data);

		/*
		* ɾ�����������ж���ͱߵ��±�λ��
		* ����ֵ�����ر�ɾ���������һ�������±꣨���򣩣����������û����һ�����㷵�� npos
		*/
		size_t removeVertex(size_t index);

		/*
		* �����²���ߵ��±꣬����ʧ�ܷ��� npos
		*/
		size_t insertEdge(size_t v1, size_t v2, const E& data);

		/*
		* �����²���ߵ��±꣬����ʧ�ܷ��� npos
		*/
		size_t insertEdgeWithVertexData(const V& v1Data, const V& v2Data, const E& data);

		/*
		* ɾ�����������бߺͶ�����±�λ��
		*/
		void removeEdge(size_t v1, size_t v2);

		/*
		* ɾ�����������бߺͶ�����±�λ��
		* ����ֵ�����ر�ɾ���ߵ���һ�����±꣨���򣩣����������û����һ���߷��� npos
		*/
		size_t removeEdge(size_t index);

		/*
		* ����ƥ�䵽�ĵ�һ��������±꣬ƥ�䲻������ npos
		*/
		size_t getVertexIndex(const V& data) const;

		/*
		* �Ҳ������㽫���׳� std::bad_exception �쳣
		*/
		V& getVertexData(size_t index);

		/*
		* �Ҳ����߽����׳� std::bad_exception �쳣
		*/
		E& getEdgeData(size_t v1, size_t v2);

		/*
		* �Ҳ����߽����׳� std::bad_exception �쳣
		*/
		E& getEdgeData(size_t index);

		bool isExistVertex(size_t index) const;

		bool isExistEdge(size_t v1, size_t v2) const;

		bool isExistEdge(size_t index) const;

		size_t degree(size_t index) const;

		void vertexDFS(size_t index, const TraverseCallback& callback);

		void vertexBFS(size_t index, const TraverseCallback& callback);

		void vertexDFS(const TraverseCallback& callback);

		void vertexBFS(const TraverseCallback& callback);

		/*
		* �ɹ����� true ���������󷵻� false �����ָ�Ȩ��С����Ȩ·����������� false
		*/
		bool ShortestPathsSPFA(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const;

		/*
		* �ɹ����� true ���������󷵻� false ������и�Ȩ�߽���õ�����Ľ������Ȩ��ͬ��Ҳ��õ�����Ľ��
		*/
		bool ShortestPathsDijkstra(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const;

		/*
		* ���ָ�Ȩ��С����Ȩ·�����������õ�����Ľ��
		*/
		void ShortestPathsFloyd(std::vector<std::vector<Weight> >& weights_out, std::vector<std::vector<size_t> >& paths_out) const;

		void MSTKruskal(std::vector<size_t>& edges_out) const;

		void MSTPrim(std::vector<size_t>& edges_out) const;

		/*
		* ����ͼ��ͨͼ Kruskal https://blog.csdn.net/qq_41754350/article/details/81460643
		* ����ͼ��ͨͼ Prim https://www.cnblogs.com/fzl194/p/8722989.html
		* 
		* �����
		* ���ڣ�����������ͨ��һ��������ʱ�����ǳ����������������ڵģ��������������������������㡣
		* ������ĳ������Ķ�����Ϊ���������ıߵ�������
		* ��ͼ������һ��ͼ�����бߵ�һ���Ӽ����Լ����������������ж��㣩��ɵ�ͼ�����������ⶼ��Ҫʶ��������͵���ͼ���ر������ܹ�˳������һϵ�ж���ı�����ɵ���ͼ��
		* ·�������ɱ�˳�����ӵ�һϵ�ж��㡣
		* ��·������һ��û���ظ������·����
		* ������һ�����ٺ���һ�����������յ���ͬ��·����
		* �򵥻�����һ�������������յ������֮ͬ�⣩�������ظ�����ͱߵĻ���
		* ���ȣ�·���򻷵ĳ���Ϊ�����������ı�����
		* ��ͨ������������֮�����һ������˫����·��ʱ�����ǳ�һ���������һ����������ͨ�ġ�����������u-v-w-x�ļǷ�����ʾu��x��һ��·������u-v-w-x-u��ʾһ������
		* ��ͨͼ�����������һ�����㶼����һ��·��������һ�����ⶥ�㣬���ǳ����ͼ����ͨͼ��
		* ����ͨ��ͼ����������ͨ������ɣ����Ƕ��Ǽ�����ͨ��ͼ��
		* �޻�ͼ����һ�ֲ���������ͼ��
		* ������һ���޻���ͨͼ��
		* ɭ�֣���������������ɵļ��ϳ�Ϊɭ�֡�
		* ����������ͨͼ��������������һ����ͼ��������ͼ�е����ж�������һ������
		* ������ɭ�֣�������������ͨ��ͼ���������ļ���
		* 
		* ���ҽ���һ������V������ͼG��������5������֮һʱ��������һ������
		* G��V-1�����Ҳ����л���
		* G��V-1����������ͨ�ģ�
		* G����ͨ�ģ���ɾ������һ���߶���ʹ��������ͨ��
		* G���޻�ͼ�����������һ���߶������һ������
		* G�е�����һ�Զ���֮�������һ����·����
		* 
		* ����ͼ��
		* ��ͨͼ������ͼ�У����������������ͨ�ģ���ôͼ��������ͨͼ��
		* 
		* ����ͼ��
		* ����ͨ������ͼ�ĵ�ͼ������ͼ������ͨͼ����������ͨͼ��
		* ������ͨ������ͼ�У���������У����ٴ�һ������һ���ǿɴ�ģ����ǵ�����ͨ��
		* ǿ��ͨ������ͼ�У�ǿ��ͨͼ��������ж�����ɴ
		*/

		std::vector<std::vector<Weight> > makeAdjacencyMatrix() const;

		size_t vertexSize() const;

		size_t edgeSize() const;

		void clear();

		static const size_t npos = static_cast<size_t>(-1);

		/*
		* ����ֵ���������
		*/
		static std::list<size_t> makePathWithResultForDijkstraAndSPFA(size_t endIndex, std::vector<Weight>& weights, std::vector<size_t>& paths);

		/*
		* ����ֵ���������
		*/
		static std::list<size_t> makePathWithResultForFloyd(size_t beginIndex, size_t endIndex, std::vector<std::vector<Weight> >& weights, std::vector<std::vector<size_t> >& paths);

	private:
		void _changeEdgeLink(const std::set<size_t>& edgeSet, size_t originIndex, size_t newIndex);
		void _changeEdgeAndVertexLink(size_t vertexIndex, size_t originIndex, size_t newIndex);

	private:
		std::vector<Vertex<V> >		m_Vertexes;
		std::vector<Edge<E> >		m_Edges;
		std::map<EdgeKey, size_t>	m_EdgeKeys;
	};

	template<typename V, typename E>
	GraphUndirected<V, E>::GraphUndirected(size_t vertexCapacity /*= 256*/, size_t edgeCapacity /*= 256*/)
	{
		m_Vertexes.reserve(vertexCapacity);
		m_Edges.reserve(edgeCapacity);
	}

	template<typename V, typename E>
	GraphUndirected<V, E>::GraphUndirected(const GraphUndirected& other)
	{
		m_Vertexes = other.m_Vertexes;
		m_Edges = other.m_Edges;
		m_EdgeKeys = other.m_EdgeKeys;
	}

	template<typename V, typename E>
	GraphUndirected<V, E>::GraphUndirected(GraphUndirected&& right)
	{
		m_Vertexes = std::move(right.m_Vertexes);
		m_Edges = std::move(right.m_Edges);
		m_EdgeKeys = std::move(right.m_EdgeKeys);
	}

	template<typename V, typename E>
	GraphUndirected<V, E>::~GraphUndirected()
	{
		clear();
	}

	template<typename V, typename E>
	GraphUndirected<V, E>& GraphUndirected<V, E>::operator=(const GraphUndirected& other)
	{
		m_Vertexes = other.m_Vertexes;
		m_Edges = other.m_Edges;
		m_EdgeKeys = other.m_EdgeKeys;
		return *this;
	}

	template<typename V, typename E>
	GraphUndirected<V, E>& GraphUndirected<V, E>::operator=(GraphUndirected&& right)
	{
		m_Vertexes = std::move(right.m_Vertexes);
		m_Edges = std::move(right.m_Edges);
		m_EdgeKeys = std::move(right.m_EdgeKeys);
		return *this;
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::insertVertex(const V& data)
	{
		Vertex<V> vertex;
		vertex.data = data;
		m_Vertexes.push_back(std::move(vertex));
		return m_Vertexes.size() - 1;
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::removeVertex(size_t index)
	{
		size_t next = npos;
		if (!isExistVertex(index))
			return next;
		Vertex<V>& vertex = m_Vertexes[index];
		while (vertex.edges.size() > 0)
			removeEdge(*vertex.edges.begin());
		if (m_Vertexes.size() > 1)
		{
			if (index != m_Vertexes.size() - 1)
			{
				next = index;
				m_Vertexes[index] = std::move(m_Vertexes[m_Vertexes.size() - 1]);
				_changeEdgeLink(vertex.edges, m_Vertexes.size() - 1, index);
			}
			m_Vertexes.pop_back();
		}
		else
			m_Vertexes.clear();
		return next;
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::insertEdge(size_t v1, size_t v2, const E& data)
	{
		if (!isExistVertex(v1) || !isExistVertex(v2) || v1 == v2)
			return npos;
		EdgeKey edgeKey = { v1, v2 };
		if (m_EdgeKeys.find(edgeKey) != m_EdgeKeys.end())
			return npos;
		Edge<E> edge;
		edge.data = data;
		edge.edgeKey = edgeKey;
		m_Edges.push_back(std::move(edge));
		size_t edgeIndex = m_Edges.size() - 1;
		m_EdgeKeys[edgeKey] = edgeIndex;
		m_Vertexes[v1].edges.insert(edgeIndex);
		m_Vertexes[v2].edges.insert(edgeIndex);
		return edgeIndex;
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::insertEdgeWithVertexData(const V& v1Data, const V& v2Data, const E& data)
	{
		size_t v1 = npos;
		size_t v2 = npos;
		for (size_t i = 0; i < m_Vertexes.size() && (v1 == npos || v2 == npos); i++)
		{
			if (v1 == npos && m_Vertexes[i].data == v1Data)
				v1 = i;
			else if (v2 == npos && m_Vertexes[i].data == v2Data)
				v2 = i;
		}
		if (v1 == npos || v2 == npos)
			return npos;
		return insertEdge(v1, v2, data);
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::removeEdge(size_t v1, size_t v2)
	{
		EdgeKey edgeKey = { v1, v2 };
		std::map<EdgeKey, size_t>::iterator it = m_EdgeKeys.find(edgeKey);
		if (it == m_EdgeKeys.end())
			return;
		removeEdge(it->second);
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::removeEdge(size_t index)
	{
		size_t next = npos;
		if (!isExistEdge(index))
			return next;
		Edge<E>& edge = m_Edges[index];
		m_Vertexes[edge.edgeKey.v1].edges.erase(index);
		m_Vertexes[edge.edgeKey.v2].edges.erase(index);
		if (m_Edges.size() > 1)
		{
			m_EdgeKeys.erase(edge.edgeKey);
			if (index != m_Edges.size() - 1)
			{
				next = index;
				m_EdgeKeys[m_Edges[m_Edges.size() - 1].edgeKey] = index;
				m_Edges[index] = std::move(m_Edges[m_Edges.size() - 1]);
				_changeEdgeAndVertexLink(m_Edges[index].edgeKey.v1, m_Edges.size() - 1, index);
				_changeEdgeAndVertexLink(m_Edges[index].edgeKey.v2, m_Edges.size() - 1, index);
			}
			m_Edges.pop_back();
		}
		else
		{
			m_Edges.clear();
			m_EdgeKeys.clear();
		}
		return next;
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::getVertexIndex(const V& data) const
	{
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			if (m_Vertexes[i].data == data)
				return i;
		return npos;
	}

	template<typename V, typename E>
	V& GraphUndirected<V, E>::getVertexData(size_t index)
	{
		if (!isExistVertex(index))
			throw std::bad_exception();
		return m_Vertexes[index].data;
	}

	template<typename V, typename E>
	E& GraphUndirected<V, E>::getEdgeData(size_t v1, size_t v2)
	{
		EdgeKey edgeKey = { v1, v2 };
		std::map<EdgeKey, size_t>::iterator it = m_EdgeKeys.find(edgeKey);
		if (it == m_EdgeKeys.end())
			throw std::bad_exception();
		return getEdgeData(it->second);
	}

	template<typename V, typename E>
	E& GraphUndirected<V, E>::getEdgeData(size_t index)
	{
		if (!isExistEdge(index))
			throw std::bad_exception();
		return m_Edges[index].data;
	}

	template<typename V, typename E>
	bool GraphUndirected<V, E>::isExistVertex(size_t index) const
	{
		return index < m_Vertexes.size();
	}

	template<typename V, typename E>
	bool GraphUndirected<V, E>::isExistEdge(size_t v1, size_t v2) const
	{
		EdgeKey edgeKey = { v1, v2 };
		return m_EdgeKeys.find(edgeKey) != m_EdgeKeys.end();
	}

	template<typename V, typename E>
	bool GraphUndirected<V, E>::isExistEdge(size_t index) const
	{
		return index < m_Edges.size();
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::degree(size_t index) const
	{
		if (!isExistVertex(index))
			return 0;
		return m_Vertexes[index].edges.size();
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::vertexDFS(size_t index, const TraverseCallback& callback)
	{
		if (!isExistVertex(index))
			return;
		std::set<size_t> already;
		std::stack<size_t> children;
		children.push(index);
		while (children.size() > 0)
		{
			size_t idx = children.top();
			children.pop();
			std::pair<std::set<size_t>::iterator, bool> r = already.insert(idx);
			if (!r.second)
				continue;
			Vertex<V>& vertex = m_Vertexes[idx];
			if (callback(idx, vertex.data))
				break;
			for (std::set<size_t>::reverse_iterator it = vertex.edges.rbegin(); it != vertex.edges.rend(); it++)
				children.push(m_Edges[*it].edgeKey.v1 == idx ? m_Edges[*it].edgeKey.v2 : m_Edges[*it].edgeKey.v1);
		}
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::vertexBFS(size_t index, const TraverseCallback& callback)
	{
		if (!isExistVertex(index))
			return;
		std::set<size_t> already;
		std::queue<size_t> children;
		children.push(index);
		while (children.size() > 0)
		{
			size_t idx = children.front();
			children.pop();
			std::pair<std::set<size_t>::iterator, bool> r = already.insert(idx);
			if (!r.second)
				continue;
			Vertex<V>& vertex = m_Vertexes[idx];
			if (callback(idx, vertex.data))
				break;
			for (std::set<size_t>::iterator it = vertex.edges.begin(); it != vertex.edges.end(); it++)
				children.push(m_Edges[*it].edgeKey.v1 == idx ? m_Edges[*it].edgeKey.v2 : m_Edges[*it].edgeKey.v1);
		}
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::vertexDFS(const TraverseCallback& callback)
	{
		if (m_Vertexes.size() == 0)
			return;
		std::set<size_t> vertexIndexes;
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			vertexIndexes.insert(i);
		TraverseCallback cb = [&callback, &vertexIndexes](size_t index, V& data)->bool {
			if (vertexIndexes.erase(index) == 0)
				return false;
			return callback(index, data);
		};
		while (vertexIndexes.size() > 0)
			vertexDFS(*vertexIndexes.begin(), cb);
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::vertexBFS(const TraverseCallback& callback)
	{
		if (m_Vertexes.size() == 0)
			return;
		std::set<size_t> vertexIndexes;
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			vertexIndexes.insert(i);
		TraverseCallback cb = [&callback, &vertexIndexes](size_t index, V& data)->bool {
			if (vertexIndexes.erase(index) == 0)
				return false;
			return callback(index, data);
		};
		while (vertexIndexes.size() > 0)
			vertexBFS(*vertexIndexes.begin(), cb);
	}

	template<typename V, typename E>
	bool GraphUndirected<V, E>::ShortestPathsSPFA(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const
	{
		weights_out.clear();
		paths_out.clear();
		if (!isExistVertex(index))
			return false;
		weights_out.resize(m_Vertexes.size(), WEIGHT_MAX);
		paths_out.resize(m_Vertexes.size(), npos);
		struct OutInfo {
			size_t from;
			size_t index;
			Weight weight;
		};
		std::queue<OutInfo> children;
		weights_out[index] = 0;
		paths_out[index] = index;
		for (std::set<size_t>::const_iterator it = m_Vertexes[index].edges.cbegin(); it != m_Vertexes[index].edges.cend(); it++)
		{
			if (m_Edges[*it].edgeKey.v1 != index)
				children.push({ m_Edges[*it].edgeKey.v2, m_Edges[*it].edgeKey.v1, m_Edges[*it].getWeight() });
			else
				children.push({ m_Edges[*it].edgeKey.v1, m_Edges[*it].edgeKey.v2, m_Edges[*it].getWeight() });
		}
		std::vector<size_t> times;
		times.resize(m_Vertexes.size(), 0);
		while (children.size() > 0)
		{
			OutInfo outInfo = children.front();
			children.pop();
			if (weights_out[outInfo.from] + outInfo.weight < weights_out[outInfo.index])
			{
				const Vertex<V>& vertex = m_Vertexes[outInfo.index];
				if (++times[outInfo.index] == m_Vertexes.size())
					return false; // ��Ȩ��С����Ȩ·���γ���ѭ��
				weights_out[outInfo.index] = weights_out[outInfo.from] + outInfo.weight;
				paths_out[outInfo.index] = outInfo.from;
				for (std::set<size_t>::const_iterator it = vertex.edges.cbegin(); it != vertex.edges.cend(); it++)
				{
					if (m_Edges[*it].edgeKey.v1 != outInfo.index)
						children.push({ m_Edges[*it].edgeKey.v2, m_Edges[*it].edgeKey.v1, m_Edges[*it].getWeight() });
					else
						children.push({ m_Edges[*it].edgeKey.v1, m_Edges[*it].edgeKey.v2, m_Edges[*it].getWeight() });
				}
			}
		}
		return true;
	}

	template<typename V, typename E>
	bool GraphUndirected<V, E>::ShortestPathsDijkstra(size_t index, std::vector<Weight>& weights_out, std::vector<size_t>& paths_out) const
	{
		weights_out.clear();
		paths_out.clear();
		if (!isExistVertex(index))
			return false;
		weights_out.resize(m_Vertexes.size(), WEIGHT_MAX);
		paths_out.resize(m_Vertexes.size(), npos);
		std::vector<bool> checked(m_Vertexes.size(), false);
		weights_out[index] = 0;
		paths_out[index] = index;
		checked[index] = true;
		struct OutInfo {
			size_t from;
			size_t index;
			Weight weight;
			bool operator<(const OutInfo& other) const
			{
				return weight > other.weight; //�����õ����ѣ��������ﷴ���ж�
			}
		};
		std::priority_queue<OutInfo> heap_vertexes;
		for (std::set<size_t>::const_iterator it = m_Vertexes[index].edges.cbegin(); it != m_Vertexes[index].edges.cend(); it++)
			heap_vertexes.push({ index, m_Edges[*it].edgeKey.v1 == index ? m_Edges[*it].edgeKey.v2 : m_Edges[*it].edgeKey.v1, m_Edges[*it].getWeight() });
		while (heap_vertexes.size() > 0)
		{
			OutInfo outInfo = heap_vertexes.top();
			heap_vertexes.pop();
			if (checked[outInfo.index])
				continue;
			checked[outInfo.index] = true;
			if (outInfo.weight < weights_out[outInfo.index])
			{
				weights_out[outInfo.index] = outInfo.weight;
				paths_out[outInfo.index] = outInfo.from;
				const Vertex<V>& vertex = m_Vertexes[outInfo.index];
				for (std::set<size_t>::const_iterator it = vertex.edges.cbegin(); it != vertex.edges.cend(); it++)
				{
					size_t to = m_Edges[*it].edgeKey.v1 == outInfo.index ? m_Edges[*it].edgeKey.v2 : m_Edges[*it].edgeKey.v1;
					if (!checked[to])
						heap_vertexes.push({ outInfo.index, to, weights_out[outInfo.index] + m_Edges[*it].getWeight() });
				}
			}
		}
		return true;
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::ShortestPathsFloyd(std::vector<std::vector<Weight> >& weights_out, std::vector<std::vector<size_t> >& paths_out) const
	{
		weights_out.clear();
		paths_out.clear();
		if (m_Vertexes.size() == 0)
			return;
		paths_out.resize(m_Vertexes.size(), std::vector<size_t>(m_Vertexes.size(), npos));
		std::vector<std::vector<Weight> > adjMatrix = makeAdjacencyMatrix();
		for (size_t k = 0; k < m_Vertexes.size(); k++)
			for (size_t i = 0; i < m_Vertexes.size(); i++)
				for (size_t j = 0; j < m_Vertexes.size(); j++)
					if (adjMatrix[i][k] != WEIGHT_MAX && adjMatrix[k][j] != WEIGHT_MAX && adjMatrix[i][j] > adjMatrix[i][k] + adjMatrix[k][j])
					{
						adjMatrix[i][j] = adjMatrix[i][k] + adjMatrix[k][j];
						paths_out[i][j] = k;
					}
		weights_out = std::move(adjMatrix);
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::MSTKruskal(std::vector<size_t>& edges_out) const
	{
		edges_out.clear();
		if (m_Vertexes.size() == 0)
			return;
		struct EdgeInfo {
			size_t index;
			Weight weight;
			bool operator>(const EdgeInfo& other) const
			{
				return weight > other.weight;
			}
		};
		std::priority_queue<EdgeInfo, std::vector<EdgeInfo>, std::greater<EdgeInfo> > edgeHeap;
		for (size_t i = 0; i < m_Edges.size(); i++)
			edgeHeap.push({ i,m_Edges[i].getWeight() });
		std::vector<size_t> already;
		already.resize(m_Vertexes.size(), npos);
		size_t joinCount = 0;
		while (edgeHeap.size() > 0)
		{
			EdgeInfo edgeInfo = edgeHeap.top();
			edgeHeap.pop();
			const Edge<E>& edge = m_Edges[edgeInfo.index];
			size_t v1End = edge.edgeKey.v1;
			while (already[v1End] != npos)
				v1End = already[v1End];
			size_t v2End = edge.edgeKey.v2;
			while (already[v2End] != npos)
				v2End = already[v2End];
			if (v1End != v2End) // ����ͬһ����
			{
				already[v1End] = v2End;
				edges_out.push_back(edgeInfo.index);
				if (++joinCount == m_Vertexes.size() - 1)
					return;
			}
		}
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::MSTPrim(std::vector<size_t>& edges_out) const
	{
		edges_out.clear();
		if (m_Vertexes.size() == 0)
			return;
		struct EdgeInfo {
			size_t index;
			Weight weight;
			bool operator>(const EdgeInfo& other) const
			{
				return weight > other.weight;
			}
		};
		std::priority_queue<EdgeInfo, std::vector<EdgeInfo>, std::greater<EdgeInfo> > edgeHeap;
		std::vector<bool> already;
		already.resize(m_Vertexes.size(), false);
		already[0] = true;
		for (std::set<size_t>::const_iterator it = m_Vertexes[0].edges.cbegin(); it != m_Vertexes[0].edges.cend(); it++)
			edgeHeap.push({ *it,m_Edges[*it].getWeight() });
		while (edgeHeap.size() > 0)
		{
			EdgeInfo edgeInfo = edgeHeap.top();
			edgeHeap.pop();
			const EdgeKey& edgeKey = m_Edges[edgeInfo.index].edgeKey;
			size_t vi = edgeKey.v1;
			if (already[edgeKey.v1])
			{
				if (already[edgeKey.v2])
					continue;
				vi = edgeKey.v2;
			}
			already[vi] = true;
			for (std::set<size_t>::const_iterator it = m_Vertexes[vi].edges.cbegin(); it != m_Vertexes[vi].edges.cend(); it++)
			{
				const Edge<E>& edge = m_Edges[*it];
				if (!already[edge.edgeKey.v1] || !already[edge.edgeKey.v2])
					edgeHeap.push({ *it,edge.getWeight() });
			}
			edges_out.push_back(edgeInfo.index);
		}
	}

	template<typename V, typename E>
	std::vector<std::vector<Weight> > GraphUndirected<V, E>::makeAdjacencyMatrix() const
	{
		std::vector<std::vector<Weight> > adjMatrix;
		adjMatrix.resize(m_Vertexes.size(), std::vector<Weight>(m_Vertexes.size(), WEIGHT_MAX));
		for (size_t i = 0; i < m_Vertexes.size(); i++)
			adjMatrix[i][i] = 0;
		for (typename std::vector<Edge<E> >::const_iterator it = m_Edges.cbegin(); it != m_Edges.cend(); it++)
		{
			adjMatrix[it->edgeKey.v1][it->edgeKey.v2] = it->getWeight();
			adjMatrix[it->edgeKey.v2][it->edgeKey.v1] = it->getWeight();
		}
		return std::move(adjMatrix);
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::vertexSize() const
	{
		return m_Vertexes.size();
	}

	template<typename V, typename E>
	size_t GraphUndirected<V, E>::edgeSize() const
	{
		return m_Edges.size();
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::clear()
	{
		m_Vertexes.clear();
		m_Edges.clear();
		m_EdgeKeys.clear();
	}

	template<typename V, typename E>
	std::list<size_t> GraphUndirected<V, E>::makePathWithResultForDijkstraAndSPFA(size_t endIndex, std::vector<Weight>& weights, std::vector<size_t>& paths)
	{
		std::list<size_t> path;
		if (endIndex >= weights.size() || weights[endIndex] == WEIGHT_MAX)
			return path;
		size_t index = endIndex;
		do {
			path.push_front(index);
			index = paths[index];
			if (index == paths[index])
				break;
		} while (true);
		return path;
	}

	template<typename V, typename E>
	std::list<size_t> GraphUndirected<V, E>::makePathWithResultForFloyd(size_t beginIndex, size_t endIndex, std::vector<std::vector<Weight> >& weights, std::vector<std::vector<size_t> >& paths)
	{
		std::list<size_t> path;
		if (beginIndex >= weights.size() || endIndex >= weights[beginIndex].size() || weights[beginIndex][endIndex] == WEIGHT_MAX)
			return path;
		struct OpInfo {
			size_t from;
			size_t to;
			size_t jump;
		};
		std::stack<OpInfo> opStack;
		opStack.push({ beginIndex, endIndex, paths[beginIndex][endIndex] });
		while (opStack.size() > 0)
		{
			OpInfo opInfo = opStack.top();
			opStack.pop();
			if (opInfo.jump == npos)
			{
				path.push_back(opInfo.to);
				continue;
			}
			opStack.push({ opInfo.jump, opInfo.to, paths[opInfo.jump][opInfo.to] });
			opStack.push({ opInfo.from, opInfo.jump, paths[opInfo.from][opInfo.jump] });
		}
		return path;
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::_changeEdgeLink(const std::set<size_t>& edgeSet, size_t originIndex, size_t newIndex)
	{
		for (std::set<size_t>::const_iterator it = edgeSet.cbegin(); it != edgeSet.cend(); it++)
		{
			Edge<E>& edge = m_Edges[*it];
			m_EdgeKeys.erase(edge.edgeKey);
			if (edge.edgeKey.v1 == originIndex)
				edge.edgeKey.v1 = newIndex;
			else if (edge.edgeKey.v2 == originIndex)
				edge.edgeKey.v2 = newIndex;
			m_EdgeKeys[edge.edgeKey] = *it;
		}
	}

	template<typename V, typename E>
	void GraphUndirected<V, E>::_changeEdgeAndVertexLink(size_t vertexIndex, size_t originIndex, size_t newIndex)
	{
		Vertex<V>& vertex = m_Vertexes[vertexIndex];
		vertex.edges.erase(originIndex);
		vertex.edges.insert(newIndex);
	}

}


FW_NS_END


#endif // !__FRAMEWORK_GRAPH_H__