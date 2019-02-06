#pragma once

#include "../graph/graph.h"
#include <random>

#include <iostream>

class GenerationParams;

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
		bool _is_directed = false;

		// global non-interpreted model properties found in .gml file

		// initial properties read from input file
		VertexPropertyMap<Valence> _valence;
		VertexPropertyMap<double> _expression_threshold;
		VertexPropertyMap<double> _inner_confidence;
		VertexPropertyMap<bool> _is_bot;

	public:
		std::map<std::string,std::string> global_properties;

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
			return std::max(2.0 / (1.0 + exp((-1.0) * inner_confidence(v))) - 1.0, 0.0);
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
		
		inline std::vector<std::pair<std::string,std::string>> all_global_property_strs() const
		{
			std::vector<std::pair<std::string,std::string>> rtn;
			rtn.emplace_back("bot_influence", std::to_string(_bot_influence));
			rtn.emplace_back("alpha", std::to_string(_alpha));
			rtn.emplace_back("is_directed", std::to_string(_is_directed));

			for (auto p : global_properties) {
				rtn.push_back(p);
			}

			return rtn;
		}

		static Model read_from_gml(std::istream& istr);
		void write_to_gml(std::ostream& ostr);

		friend Model generate(GenerationParams,std::mt19937&);
};

