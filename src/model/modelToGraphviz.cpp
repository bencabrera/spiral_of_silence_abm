#include "modelToGraphviz.h"
#include <boost/graph/graphviz.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace {
	std::string double_to_str(double d)
	{
		std::stringstream ss;
		ss << std::fixed << std::setprecision(3) << d;
		return ss.str();
	}

	class VertexWriter {
		public:
			VertexWriter(const Model& m,GraphvizOptions options) 
				: _m(m), _options(options)
			{}

			void operator()(std::ostream& out, const Vertex& v) const 
			{
				out << "[shape=\"circle\",style=filled";

				std::string label;
				if(_options.show_bots)
					label += _m.is_bot(v) ? "\nB" : "";
				if(_options.show_confidence)
					label += "\nc=" + double_to_str(_m.confidence(v));
				if(_options.show_expression_threshold)
					label += "\ne=" + double_to_str(_m.expression_threshold(v));
				if(_options.show_inner_confidence)
					label += "\nc_i=" + double_to_str(_m.inner_confidence(v));

				boost::trim(label);

				if(!label.empty())
					out << ",label=\"" << label << "\"";

				if(_m.is_silenced(v))
					out << ",fillcolor=gray";
				else 
				{
					if(_m.valence(v) == GREEN)
						out << ",fillcolor=green";
					if(_m.valence(v) == RED)
						out << ",fillcolor=red";
				}

				out << "]";
			}

		private:
			const Model& _m;
			const GraphvizOptions _options;
	};
}

void draw_to_graphviz(std::ostream& ostr, const Model& m, GraphvizOptions options)
{
	boost::write_graphviz(ostr, m.graph(), VertexWriter(m,options));
}
