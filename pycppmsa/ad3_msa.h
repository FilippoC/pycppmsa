#pragma once

#include <vector>

void RunChuLiuEdmondsIteration(
        std::vector<bool> *disabled,
        std::vector<std::vector<int> > *candidate_heads,
        std::vector<std::vector<float> > *candidate_scores,
        std::vector<int> *heads,
        float *value
);

template <class T>
std::vector<int> msa(const int n_vertices, T&& weight_callback)
{
    std::vector<std::vector<int> > candidate_heads(n_vertices);
    std::vector<std::vector<float> > candidate_scores(n_vertices);
    std::vector<bool> disabled(n_vertices, false);
    for (int m = 1; m < n_vertices; ++m) {
        for (int h = 0; h < n_vertices; ++h) {
            if (h == m)
                continue;
            candidate_heads[m].push_back(h);
            candidate_scores[m].push_back(weight_callback(h, m));
        }
    }

    std::vector<int> heads(n_vertices);

    float value = 0;
    RunChuLiuEdmondsIteration(&disabled, &candidate_heads,
                              &candidate_scores, &heads,
                              &value);

    (heads)[0] = -1;

    return heads;
}
