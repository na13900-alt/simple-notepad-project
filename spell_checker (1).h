#ifndef SPELL_CHECKER_H
#define SPELL_CHECKER_H

#include <algorithm>
#include <cctype>
#include <fstream>
#include <set>
#include <string>
#include <vector>

class spell_checker {
public:
    explicit spell_checker(const std::string& word_list_path)
    {
        std::ifstream file(word_list_path);
        std::string word;
        while (std::getline(file, word)) {
            if (!word.empty()) {
                words.insert(word);
            }
        }
    }

    [[nodiscard]] bool is_correct(const std::string& word) const
    {
        std::string cleaned = clean(word);
        if (cleaned.empty()) {
            return true;
        }
        return words.count(cleaned) > 0;
    }

    [[nodiscard]] std::vector<std::string> suggestions(const std::string& word, int max = 5) const
    {
        const std::string target = clean(word);
        if (target.empty()) {
            return {};
        }

        std::vector<std::pair<int, std::string>> candidates;

        // Collect words that share the first letter and have similar length
        for (const auto& w : words) {
            if (w.empty()) {
                continue;
            }
            const int len_diff = std::abs(static_cast<int>(w.size()) - static_cast<int>(target.size()));
            if (len_diff > 3) {
                continue;
            }
            if (w[0] != target[0]) {
                continue;
            }
            const int dist = edit_distance(target, w);
            if (dist <= 2) {
                candidates.emplace_back(dist, w);
            }
        }

        std::sort(candidates.begin(), candidates.end());

        std::vector<std::string> result;
        for (int i = 0; i < static_cast<int>(candidates.size()) && i < max; ++i) {
            result.push_back(candidates[i].second);
        }
        return result;
    }

    [[nodiscard]] bool loaded() const { return !words.empty(); }

private:
    std::set<std::string> words;

    [[nodiscard]] static std::string clean(const std::string& word)
    {
        std::string result;
        for (const unsigned char ch : word) {
            if (std::isalpha(ch)) {
                result += static_cast<char>(std::tolower(ch));
            }
        }
        return result;
    }

    [[nodiscard]] static int edit_distance(const std::string& a, const std::string& b)
    {
        const std::size_t m = a.size();
        const std::size_t n = b.size();
        std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));
        for (std::size_t i = 0; i <= m; ++i) {
            dp[i][0] = static_cast<int>(i);
        }
        for (std::size_t j = 0; j <= n; ++j) {
            dp[0][j] = static_cast<int>(j);
        }
        for (std::size_t i = 1; i <= m; ++i) {
            for (std::size_t j = 1; j <= n; ++j) {
                if (a[i - 1] == b[j - 1]) {
                    dp[i][j] = dp[i - 1][j - 1];
                } else {
                    dp[i][j] = 1 + std::min({ dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1] });
                }
            }
        }
        return dp[m][n];
    }
};

#endif // SPELL_CHECKER_H
