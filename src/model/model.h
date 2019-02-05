#pragma once

#include "../graph/graph.h"

#include <iostream>

enum Valence {
	GREEN,
	RED
};

class Model 
{
	private:
		Graph g;

		// global model parameters
		double _bot_influence = 1.0;
		double _alpha = 1.0;

		// initial properties read from input file
		VertexPropertyMap<Valence> _valence;
		VertexPropertyMap<double> _expression_threshold;
		VertexPropertyMap<double> _inner_confidence;
		VertexPropertyMap<bool> _is_bot;

	public:

		// helper methods
		inline const Graph& graph() const
		{
			return g;
		}

		inline double bot_influence() const
		{
			return _bot_influence;
		}
		inline double alpha() const
		{
			return _alpha;
		}

		inline Valence valence(Vertex v) const
		{
			return _valence[v];
		}

		inline double expression_threshold(Vertex v) const
		{
			return _expression_threshold[v];
		}

		inline double& inner_confidence(Vertex v)
		{
			return _inner_confidence[v];
		}

		inline double inner_confidence(Vertex v) const
		{
			return _inner_confidence[v];
		}

		inline bool is_bot(Vertex v) const
		{
			return _is_bot[v];
		}

		inline double confidence(Vertex v) const
		{
			return 2.0 / (1.0 + exp((-1.0) * inner_confidence(v))) - 1.0;
		}

		inline bool is_silenced(Vertex v) const
		{
			return confidence(v) <= expression_threshold(v);
		}

		inline bool is_speaking_out(Vertex v) const
		{
			return !is_silenced(v);
		}

		// (helper) properties that are set after initialization of while running simulation

		static Model read_from_gml(std::istream& istr);
};

