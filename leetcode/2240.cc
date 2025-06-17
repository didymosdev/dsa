// https://leetcode.com/problems/intervals-between-identical-elements/description/

/*
You are given a 0-indexed array of n integers arr.

The interval between two elements in arr is defined as the absolute difference
between their indices. More formally, the interval between arr[i] and arr[j] is
|i - j|.

Return an array intervals of length n where intervals[i] is the sum of intervals
between arr[i] and each element in arr with the same value as arr[i].

Note: |x| is the absolute value of x.

 
Example 1:


Input: arr = [2,1,3,1,2,3,3]
Output: [4,2,7,2,4,4,5]
Explanation:
- Index 0: Another 2 is found at index 4. |0 - 4| = 4
- Index 1: Another 1 is found at index 3. |1 - 3| = 2
- Index 2: Two more 3s are found at indices 5 and 6. |2 - 5| + |2 - 6| = 7
- Index 3: Another 1 is found at index 1. |3 - 1| = 2
- Index 4: Another 2 is found at index 0. |4 - 0| = 4
- Index 5: Two more 3s are found at indices 2 and 6. |5 - 2| + |5 - 6| = 4
- Index 6: Two more 3s are found at indices 2 and 5. |6 - 2| + |6 - 5| = 5


Example 2:


Input: arr = [10,5,10,10]
Output: [5,0,3,4]
Explanation:
- Index 0: Two more 10s are found at indices 2 and 3. |0 - 2| + |0 - 3| = 5
- Index 1: There is only one 5 in the array, so its sum of intervals to
identical elements is 0.
- Index 2: Two more 10s are found at indices 0 and 3. |2 - 0| + |2 - 3| = 3
- Index 3: Two more 10s are found at indices 0 and 2. |3 - 0| + |3 - 2| = 4


 
Constraints:


        n == arr.length
        1 <= n <= 105
        1 <= arr[i] <= 105


 
Note: This question is the same as  2615: Sum of Distances.
*/

#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

class Solution {
 public:
  vector<long long> getDistances(vector<int>& arr) {
    vector<long long> ans(arr.size(), 0);
    unordered_map<int, vector<int>> m;
    for (int i = 0; i < arr.size(); i++) {
      m[arr[i]].push_back(i);
    }
    for (const auto& [k, v] : m) {
      long long sum = 0;
      for (const auto& i : v) {
        sum += i;
      }
      long long prefix = 0;
      for (long long i = 0; i < v.size(); i++) {
        long long lsb = (v[i] * i - prefix);
        long long msb = (sum - prefix - v[i]) - (v[i] * (v.size() - i - 1));
        ans[v[i]] = lsb + msb;
        prefix += v[i];
      }
    }
    return ans;
  }
};

int main() {
  Solution sol;
  vector<int> arr;
  arr = {2, 1, 3, 1, 2, 3, 3};
  for (auto ans : sol.getDistances(arr)) {
    printf("%lld ", ans);
  }
  puts("");
  arr = {10, 5, 10, 10};
  for (auto ans : sol.getDistances(arr)) {
    printf("%lld ", ans);
  }
  puts("");
  return 0;
}
