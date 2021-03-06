// This is from AD3: AD3/examples/cpp/parsing/FactorTree.cpp
// Edited by Caio Corro for this project

// Copyright (c) 2012 Andre Martins
// All Rights Reserved.
//
// This file is part of AD3 2.1.
//
// AD3 2.1 is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// AD3 2.1 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with AD3 2.1.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <utility>
#include <iterator>
#include <cstdlib>
#include <limits>
#include <cerrno>

#include "ad3_msa.h"

void RunChuLiuEdmondsIteration(
    std::vector<bool> *disabled,
    std::vector<std::vector<int> > *candidate_heads,
    std::vector<std::vector<float> > *candidate_scores,
    std::vector<int> *heads,
    float *value
) {
    // Original number of nodes (including the root).
    int length = disabled->size();

    // Pick the best incoming arc for each node.
    heads->resize(length);
    std::vector<float> best_scores(length);
    for (int m = 1; m < length; ++m) {
        if ((*disabled)[m]) continue;
        int best = -1;
        for (int k = 0; k < (*candidate_heads)[m].size(); ++k) {
            if (best < 0 ||
                (*candidate_scores)[m][k] > (*candidate_scores)[m][best]) {
                best = k;
            }
        }
        if (best < 0) {
            // No spanning tree exists. Assign the parent of this node
            // to the root, and give it a minus infinity score.
            (*heads)[m] = 0;
            best_scores[m] = -std::numeric_limits<float>::infinity();
        } else {
            (*heads)[m] = (*candidate_heads)[m][best]; //best;
            best_scores[m] = (*candidate_scores)[m][best]; //best;
        }
    }

    // Look for cycles. Return after the first cycle is found.
    std::vector<int> cycle;
    std::vector<int> visited(length, 0);
    for (int m = 1; m < length; ++m) {
        if ((*disabled)[m]) continue;
        // Examine all the ancestors of m until the root or a cycle is found.
        int h = m;
        while (h != 0) {
            // If already visited, break and check if it is part of a cycle.
            // If visited[h] < m, the node was visited earlier and seen not
            // to be part of a cycle.
            if (visited[h]) break;
            visited[h] = m;
            h = (*heads)[h];
        }

        // Found a cycle to which h belongs.
        // Obtain the full cycle.
        if (visited[h] == m) {
            m = h;
            do {
                cycle.push_back(m);
                m = (*heads)[m];
            } while (m != h);
            break;
        }
    }

    // If there are no cycles, then this is a well formed tree.
    if (cycle.empty()) {
        *value = 0.0;
        for (int m = 1; m < length; ++m) {
            *value += best_scores[m];
        }
        return;
    }

    // Build a cycle membership vector for constant-time querying and compute the
    // score of the cycle.
    // Nominate a representative node for the cycle and disable all the others.
    float cycle_score = 0.0;
    std::vector<bool> in_cycle(length, false);
    int representative = cycle[0];
    for (int k = 0; k < cycle.size(); ++k) {
        int m = cycle[k];
        in_cycle[m] = true;
        cycle_score += best_scores[m];
        if (m != representative) (*disabled)[m] = true;
    }

    // Contract the cycle.
    // 1) Update the score of each child to the maximum score achieved by a parent
    // node in the cycle.
    std::vector<int> best_heads_cycle(length);
    for (int m = 1; m < length; ++m) {
        if ((*disabled)[m] || m == representative) continue;
        float best_score;
        // If the list of candidate parents of m is shorter than the length of
        // the cycle, use that. Otherwise, loop through the cycle.
        int best = -1;
        for (int k = 0; k < (*candidate_heads)[m].size(); ++k) {
            if (!in_cycle[(*candidate_heads)[m][k]]) continue;
            if (best < 0 || (*candidate_scores)[m][k] > best_score) {
                best = k;
                best_score = (*candidate_scores)[m][best];
            }
        }
        if (best < 0) continue;
        best_heads_cycle[m] = (*candidate_heads)[m][best];

        // Reconstruct the list of candidate heads for this m.
        int l = 0;
        for (int k = 0; k < (*candidate_heads)[m].size(); ++k) {
            int h = (*candidate_heads)[m][k];
            float score = (*candidate_scores)[m][k];
            if (!in_cycle[h]) {
                (*candidate_heads)[m][l] = h;
                (*candidate_scores)[m][l] = score;
                ++l;
            }
        }
        // If h is in the cycle and is not the representative node,
        // it will be dropped from the list of candidate heads.
        (*candidate_heads)[m][l] = representative;
        (*candidate_scores)[m][l] = best_score;
        (*candidate_heads)[m].resize(l+1);
        (*candidate_scores)[m].resize(l+1);
    }

    // 2) Update the score of each candidate parent of the cycle supernode.
    std::vector<int> best_modifiers_cycle(length, -1);
    std::vector<int> candidate_heads_representative;
    std::vector<float> candidate_scores_representative;

    std::vector<float> best_scores_cycle(length);
    // Loop through the cycle.
    for (int k = 0; k < cycle.size(); ++k) {
        int m = cycle[k];
        for (int l = 0; l < (*candidate_heads)[m].size(); ++l) {
            // Get heads out of the cycle.
            int h = (*candidate_heads)[m][l];
            if (in_cycle[h]) continue;

            float score = (*candidate_scores)[m][l] - best_scores[m];
            if (best_modifiers_cycle[h] < 0 || score > best_scores_cycle[h]) {
                best_modifiers_cycle[h] = m;
                best_scores_cycle[h] = score;
            }
        }
    }
    for (int h = 0; h < length; ++h) {
        if (best_modifiers_cycle[h] < 0) continue;
        float best_score = best_scores_cycle[h] + cycle_score;
        candidate_heads_representative.push_back(h);
        candidate_scores_representative.push_back(best_score);
    }

    // Reconstruct the list of candidate heads for the representative node.
    (*candidate_heads)[representative] = candidate_heads_representative;
    (*candidate_scores)[representative] = candidate_scores_representative;

    // Save the current head of the representative node (it will be overwritten).
    int head_representative = (*heads)[representative];

    // Call itself recursively.
    RunChuLiuEdmondsIteration(disabled,
                              candidate_heads,
                              candidate_scores,
                              heads,
                              value);

    // Uncontract the cycle.
    int h = (*heads)[representative];
    (*heads)[representative] = head_representative;
    (*heads)[best_modifiers_cycle[h]] = h;

    for (int m = 1; m < length; ++m) {
        if ((*disabled)[m]) continue;
        if ((*heads)[m] == representative) {
            // Get the right parent from within the cycle.
            (*heads)[m] = best_heads_cycle[m];
        }
    }
    for (int k = 0; k < cycle.size(); ++k) {
        int m = cycle[k];
        (*disabled)[m] = false;
    }
}
