//
// Created by egordm on 9-9-2018.
//

#ifndef PROJECT_PLOTLY_H
#define PROJECT_PLOTLY_H

#include <vector>
#include <string>
#include <nlohmann/json.hpp>
#include <armadillo>


using json = nlohmann::json;

namespace plotly {
    template<typename T>
    inline json arma_to_json(arma::Mat<T> A) {
        json ret;

        if(A.n_rows == 1 || A.n_cols == 1) {
            for (arma::uword r = 0; r < A.n_rows; ++r) {
                for (arma::uword c = 0; c < A.n_cols; ++c) ret.push_back(A.at(r, c));
            }
        } else {
            for (arma::uword r = 0; r < A.n_rows; ++r) {
                json row;
                for (arma::uword c = 0; c < A.n_cols; ++c) row.push_back(A.at(r, c));
                ret.push_back(row);
            }
        }

        return ret;
    }


    inline void gen_random(char *s, const int len) {
        static const char ALPHANUM[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static thread_local std::random_device rd;

        std::mt19937 eng(rd());
        std::uniform_int_distribution<> distr(0, sizeof(ALPHANUM) - 1);

        for (int i = 0; i < len; ++i) s[i] = ALPHANUM[distr(eng)];
        s[len] = '\0';
    }

    inline std::string generate_uuid() {
        static const int UUID_LENGTH = 36;

        char buffer[UUID_LENGTH];
        gen_random(buffer, UUID_LENGTH);

        buffer[8] = '-';
        buffer[13] = '-';
        buffer[18] = '-';
        return std::string(buffer);
    }

    class PlotlyStructure {
    public:
        std::string uuid;

        PlotlyStructure() : uuid(generate_uuid()) {}
    };

    class Chart : public PlotlyStructure {
    public:
        json attributes;

        Chart() = default;

        Chart(const std::string &name, const json &x, const json &y, const std::string &type = "scatter");

        static Chart create_heatmap(const std::string &name, const json &x, const json &y, const json &z,
                                    bool show_scale = false, const std::string &colorscale = "Viridis");

        json serialize();
    };

    class Plot : public PlotlyStructure {
    public:
        std::vector<Chart> charts;
        json attributes;

        explicit Plot(const std::string &title);

        json serialize_charts();
    };

    class File {
    public:
        std::string title;
        std::vector<Plot> plots;

        std::string to_string();

        void save(const std::string &path);
    };

};


#endif //PROJECT_PLOTLY_H
