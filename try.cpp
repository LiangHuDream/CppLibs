
const int OVER = -2;

class Solution {
public:
    struct PairHash {
        size_t operator()(const pair<int,int>& p) const {
            return ((size_t)p.first) * 13131ULL + (size_t)(p.second + 10000);
        }
    };
    
    int maxProduct(vector<int>& nums, int k, int limit) {
        unordered_map<pair<int,int>, unordered_set<int>, PairHash> dp;
        
        for (int x : nums) {
            unordered_map<pair<int,int>, unordered_set<int>, PairHash> new_dp = dp;
            for (auto &entry : dp) {
                int par = entry.first.first;
                int alt = entry.first.second;
                for (int prod : entry.second) {
                    int new_prod;
                    if (x == 0) {
                        new_prod = 0;
                    } else {
                        if (prod == 0) {
                            new_prod = 0;
                        }
                        else if (prod == OVER) {
                            new_prod = OVER;
                        } else {
                            long long candidate = (long long) prod * x;
                            if (candidate <= limit)
                                new_prod = (int) candidate;
                            else 
                                new_prod = OVER; // mark as overflow
                        }
                    }
                    int new_par = 1 - par;
                    int new_alt = (par == 0 ? alt + x : alt - x);
                    new_dp[{new_par, new_alt}].insert(new_prod);
                }
            }
            int new_start;
            if (x == 0) {
                new_start = 0;
            } else {
                if (x <= limit)
                    new_start = x;
                else
                    new_start = OVER;
            }
            new_dp[{1, x}].insert(new_start); 
            dp = move(new_dp);
        }

        int answer = -1;
        for (auto &entry : dp) {
            if (entry.first.second == k) {
                for (int prod : entry.second) {
                    if (prod != OVER)
                        answer = max(answer, prod);
                }
            }
        }
        return answer;
    }
};
