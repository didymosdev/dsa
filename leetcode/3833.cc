// https://leetcode.com/problems/merge-operations-for-minimum-travel-time/description/

/*
You are given a straight road of length l km, an integer n, an integer k, and
two integer arrays, position and time, each of length n.

The array position lists the positions (in km) of signs in strictly increasing
order (with position[0] = 0 and position[n - 1] = l).

Each time[i] represents the time (in minutes) required to travel 1 km between
position[i] and position[i + 1].

You must perform exactly k merge operations. In one merge, you can choose any
two adjacent signs at indices i and i + 1 (with i > 0 and i + 1 < n) and:


        Update the sign at index i + 1 so that its time becomes time[i] + time[i
+ 1]. Remove the sign at index i.


Return the minimum total travel time (in minutes) to travel from 0 to l after
exactly k merges.

 
Example 1:


Input: l = 10, n = 4, k = 1, position = [0,3,8,10], time = [5,8,3,6]

Output: 62

Explanation:



        Merge the signs at indices 1 and 2. Remove the sign at index 1, and
change the time at index 2 to 8 + 3 = 11.

        After the merge:

                position array: [0, 8, 10]
                time array: [5, 11, 6]







                                Segment
                                Distance (km)
                                Time per km (min)
                                Segment Travel Time (min)




                                0 → 8
                                8
                                5
                                8 × 5 = 40


                                8 → 10
                                2
                                11
                                2 × 11 = 22




        Total Travel Time: 40 + 22 = 62, which is the minimum possible time
after exactly 1 merge.



Example 2:


Input: l = 5, n = 5, k = 1, position = [0,1,2,3,5], time = [8,3,9,3,3]

Output: 34

Explanation:


        Merge the signs at indices 1 and 2. Remove the sign at index 1, and
change the time at index 2 to 3 + 9 = 12. After the merge:

                position array: [0, 2, 3, 5]
                time array: [8, 12, 3, 3]







                                Segment
                                Distance (km)
                                Time per km (min)
                                Segment Travel Time (min)




                                0 → 2
                                2
                                8
                                2 × 8 = 16


                                2 → 3
                                1
                                12
                                1 × 12 = 12


                                3 → 5
                                2
                                3
                                2 × 3 = 6




        Total Travel Time: 16 + 12 + 6 = 34, which is the minimum possible time
after exactly 1 merge.



 
Constraints:


        1 <= l <= 105
        2 <= n <= min(l + 1, 50)
        0 <= k <= min(n - 2, 10)
        position.length == n
        position[0] = 0 and position[n - 1] = l
        position is sorted in strictly increasing order.
        time.length == n
        1 <= time[i] <= 100​
        1 <= sum(time) <= 100​​​​​​
*/

#include <climits>
#include <iostream>
#include <numeric>
#include <vector>
using namespace std;

class Solution {
 public:
  int minTravelTime(int cur_pos, int cur_k, int cur_time,
                    vector<vector<vector<int>>> &dp, int l, int n, int k,
                    vector<int> &position, vector<int> &time) {
    if (cur_pos == n - 1) {
      return cur_k > 0 ? INT_MAX : 0;
    }
    if (dp[cur_pos][cur_k][cur_time] != -1) {
      return dp[cur_pos][cur_k][cur_time];
    }
    int ans = INT_MAX;
    int res = minTravelTime(cur_pos + 1, cur_k, time[cur_pos + 1], dp, l, n, k,
                            position, time);
    if (res != INT_MAX) {
      ans = min(ans,
                (position[cur_pos + 1] - position[cur_pos]) * cur_time + res);
    }
    if (cur_k > 0) {
      int merge_cnt = 0;
      int merge_time = time[cur_pos + 1];
      for (int pos = cur_pos + 2; pos < n && cur_k - merge_cnt - 1 >= 0;
           pos++) {
        merge_cnt++;
        merge_time += time[pos];
        res = minTravelTime(pos, cur_k - merge_cnt, merge_time, dp, l, n, k,
                            position, time);
        if (res != INT_MAX) {
          ans = min(ans, (position[pos] - position[cur_pos]) * cur_time + res);
        }
      }
    }
    return dp[cur_pos][cur_k][cur_time] = ans;
  }

  int minTravelTime(int l, int n, int k, vector<int> &position,
                    vector<int> &time) {
    int sum = accumulate(begin(time), end(time), 0);
    vector<vector<vector<int>>> dp(
        n + 1, vector<vector<int>>(k + 1, vector<int>(sum, -1)));
    return minTravelTime(0, k, time[0], dp, l, n, k, position, time);
  }
};

int main() {
  Solution sol;

  // Test Case 1
  vector<int> position1{0, 3, 8, 10};
  vector<int> time1{5, 8, 3, 6};
  int result1 = sol.minTravelTime(10, 4, 1, position1, time1);
  cout << "Test Case 1: " << result1 << endl;  // Expected: 62

  // Test Case 2
  vector<int> position2{0, 1, 2, 3, 5};
  vector<int> time2{8, 3, 9, 3, 3};
  int result2 = sol.minTravelTime(5, 5, 1, position2, time2);
  cout << "Test Case 2: " << result2 << endl;  // Expected: 34

  // Test Case 3: k = 0
  vector<int> position3{0, 2, 5};
  vector<int> time3{3, 4, 5};
  int result3 = sol.minTravelTime(5, 3, 0, position3, time3);
  cout << "Test Case 3: " << result3 << endl;  // Expected: 26

  // Test Case 4: n = 2, k = 0
  vector<int> position4{0, 5};
  vector<int> time4{5, 5};
  int result4 = sol.minTravelTime(5, 2, 0, position4, time4);
  cout << "Test Case 4: " << result4 << endl;  // Expected: 25

  // Test Case 5: n = 3, k = 1
  vector<int> position5{0, 2, 5};
  vector<int> time5{3, 4, 5};
  int result5 = sol.minTravelTime(5, 3, 1, position5, time5);
  cout << "Test Case 5: " << result5 << endl;  // Expected: 15

  return 0;
}
