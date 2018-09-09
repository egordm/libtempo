//
// Created by egordm on 9-9-2018.
//

#include "plotly.h"

plotly::Chart::Chart(const std::string &name, const json &x, const json &y, const std::string &type)
        : PlotlyStructure() {
    attributes["name"] = name;
    attributes["type"] = type;
    attributes["x"] = x;
    attributes["y"] = y;
}

json plotly::Chart::serialize() {
    attributes["uuid"] = uuid;
    return attributes;
}

plotly::Chart plotly::Chart::create_heatmap(const std::string &name, const json &x, const json &y, const json &z,
                                            bool show_scale, const std::string &colorscale) {
    Chart ret(name, x, y, "heatmap");
    ret.attributes["showscale"] = show_scale;
    ret.attributes["colorscale"] = colorscale;
    ret.attributes["z"] = z;
    return ret;
}

json plotly::Plot::serialize_charts() {
    json ret;
    for (auto &chart : charts) ret.push_back(chart.serialize());
    return ret;
}

plotly::Plot::Plot(const std::string &title) : PlotlyStructure() {
    attributes["title"] = title;
}

void plotly::File::save(const std::string &path) {
    std::ofstream file;
    file.open(path);
    if (!file.is_open()) {
        std::cerr << "Cant open: " << path << std::endl;
        exit(1);
    }

    file << to_string();
    file.close();
}

std::string plotly::File::to_string() {
    std::stringstream ss;
    ss << "<html><head><script src=\"https://cdn.plot.ly/plotly-1.2.0.min.js\"></script><title>";
    ss << title << "</title></head><body>\n";

    // Add plot holders
    for (auto &plot : plots) {
        ss << "<!-- " << plot.attributes["title"] << " -->\n"
           << "<div id=\"" << plot.uuid << "\" class=\"plotly-graph-div\"></div>\n";
    }

    // Add styling
    ss << "<style>.plotly-graph-div {height: 600px; width: 100% }</style>";

    // Add js
    // TODO: move to defines?
    ss << "<script>\nwindow.PLOTLYENV=window.PLOTLYENV || {};window.PLOTLYENV.BASE_URL=\"https://plot.ly\";\n\n";
    for (auto &plot : plots) {
        ss << "Plotly.plot(\n'" << plot.uuid << "',\n" << plot.serialize_charts().dump(4) << ",\n"
           << plot.attributes.dump(4) << ")\n\n";
    }

    ss << "</script></body></html>";
    return ss.str();
}
