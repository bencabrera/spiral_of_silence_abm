#include "performOneStep.h"

StepResults perform_one_step(Model& m, double epsilon)
{
	StepResults rtn;

	const auto& g = m.graph();

	// determine global opinion climate
	std::size_t n_red = 0, n_green = 0;
	for (auto v : vertices(g)) {
		if(!m.is_silenced(v))
		{
			if(m.valence(v) == GREEN)
				n_green++;
			if(m.valence(v) == RED)
				n_red++;
		}
	}

	// compute local opinion climate for each individual = tuple (n_red,ngreen)
	VertexPropertyMap<std::pair<double,double>> local_opinion_climate(boost::num_vertices(g), boost::get(boost::vertex_index,g));
	for (auto v : vertices(g)) {
		for (auto e : in_edges(v,g)) {
			auto u = boost::source(e,g);
			if(m.is_silenced(u))
				continue;

			double val = 1;
			if(m.is_bot(u))
				val = m.bot_influence();

			if(m.valence(u) == GREEN)
				local_opinion_climate[v].first += val;
			if(m.valence(u) == RED)
				local_opinion_climate[v].second += val;
		}	
	}

	VertexPropertyMap<double> deltas(boost::num_vertices(g), boost::get(boost::vertex_index, g));

	// update confidence
	for (auto v : vertices(g)) {
		auto [n_neighbor_green,n_neighbor_red] = local_opinion_climate[v];
		double n_supporter = 0, n_opponents = 0;
		if(m.valence(v) == GREEN)
		{
			n_supporter = n_neighbor_green;
			n_opponents = n_neighbor_red;
		}
		if(m.valence(v) == RED)
		{
			n_supporter = n_neighbor_red;
			n_opponents = n_neighbor_green;
		}

		double delta = 0;
		if(n_supporter + n_opponents > 0)
			delta = static_cast<double>(n_supporter - n_opponents) / (n_supporter + n_opponents);

		deltas[v] = delta;

		double old_confidence = m.confidence(v);

		m.inner_confidence(v) += m.alpha() * delta;

		double new_confidence = m.confidence(v);

		if((old_confidence <= m.expression_threshold(v) && !m.is_silenced(v)) || (old_confidence > m.expression_threshold(v) && m.is_silenced(v)))
			rtn.flipped_vertices.push_back(v);

		if(std::abs(old_confidence-new_confidence) > epsilon && !m.is_bot(v))
			rtn.n_humans_not_changed_confidence++;
	}

	return rtn;
}
