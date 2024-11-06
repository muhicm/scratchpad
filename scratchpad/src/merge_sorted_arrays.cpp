#include <iostream>
#include <vector>

void merge(std::vector<int>& nums1, int m, std::vector<int>& nums2, int n) {
    int i = m - 1; // Pointer for nums1's m elements
    int j = n - 1; // Pointer for nums2
    int k = m + n - 1; // Pointer for the end of nums1

    // Merge in reverse order
    while (j >= 0) {
        if (i >= 0 && nums1[i] > nums2[j]) {
            nums1[k] = nums1[i];
            i--;
        } else {
            nums1[k] = nums2[j];
            j--;
        }
        k--;
    }
}

int testCall() {
    std::vector<int> nums1 = {1, 2, 3, 0, 0, 0};
    int m = 3;
    std::vector<int> nums2 = {2, 5, 6};
    int n = 3;

    merge(nums1, m, nums2, n);

    // Output the merged nums1 array
    for (int num : nums1) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    return 0;
}

